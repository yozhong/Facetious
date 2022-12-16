#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageScene = new QGraphicsScene(this);
    ui->imageView->setScene(imageScene);
    ui->mainStatusBar->addPermanentWidget(ui->mainStatusLabel);
    ui->mainStatusLabel->setText("Facetious is Ready");

    listModel = new QStandardItemModel(this);
    ui->savedList->setModel(listModel);

    // connect the signals and slots
    connect(ui->actionCamera_Information, SIGNAL(triggered(bool)), this, SLOT(showCameraInfo()));
    connect(ui->actionOpen_Camera, SIGNAL(triggered(bool)), this, SLOT(openCamera()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->shutterButton, SIGNAL(clicked(bool)), this, SLOT(takePhoto()));

    dataLock = new QMutex();

    populateSavedList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showCameraInfo()
{
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    QString info = QString("Available Cameras: \n");

    foreach (const QCameraDevice &cameraInfo, cameras) {
        info += cameraInfo.id() + ": ";
        info += cameraInfo.description() + "\n";
    }
    QMessageBox::information(this, "Cameras", info);
}

void MainWindow::openCamera()
{
    int camID = 2;

    if(capturer != nullptr) {
        // if a thread is already running, stop it
        capturer->setRunning(false);
        disconnect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
        disconnect(capturer, &CaptureThread::photoTaken, this, &MainWindow::appendSavedPhoto);
        connect(capturer, &CaptureThread::finished, capturer, &CaptureThread::deleteLater);
    }

    capturer = new CaptureThread(camID, dataLock);
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::photoTaken, this, &MainWindow::appendSavedPhoto);
    capturer->start();

    ui->mainStatusLabel->setText(QString("Capturing Camera %1").arg(camID));
}

void MainWindow::updateFrame(cv::Mat* mat)
{
    dataLock->lock();
    currentFrame = *mat;
    dataLock->unlock();

    QImage frame(
        currentFrame.data,
        currentFrame.cols,
        currentFrame.rows,
        currentFrame.step,
        QImage::Format_RGB888);

    QPixmap image = QPixmap::fromImage(frame);

    imageScene->clear();
    imageScene->addPixmap(image);
    imageScene->update();

    ui->imageView->resetTransform();
    ui->imageView->setSceneRect(image.rect());
}

void MainWindow::populateSavedList()
{
    QDir dir(Utilities::getDataPath());
    QStringList nameFilters;
    nameFilters << "*.jpg";
    QFileInfoList files = dir.entryInfoList(
        nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach(QFileInfo photo, files) {
        QString name = photo.baseName();
        QStandardItem *item = new QStandardItem();
        listModel->appendRow(item);
        QModelIndex index = listModel->indexFromItem(item);
        listModel->setData(index, QPixmap(photo.absoluteFilePath()).scaledToHeight(145), Qt::DecorationRole);
        listModel->setData(index, name, Qt::DisplayRole);
    }
}

void MainWindow::appendSavedPhoto(QString name)
{
    QString photoPath = Utilities::getPhotoPath(name, "jpg");
    QStandardItem *item = new QStandardItem();
    listModel->appendRow(item);
    QModelIndex index = listModel->indexFromItem(item);
    listModel->setData(index, QPixmap(photoPath).scaledToHeight(145), Qt::DecorationRole);
    listModel->setData(index, name, Qt::DisplayRole);
}

void MainWindow::takePhoto()
{
    if(capturer != nullptr) {
        capturer->takePhoto();
    }
}
