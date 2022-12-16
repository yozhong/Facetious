#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <QTime>
#include <QtConcurrent>

#include <opencv2/opencv.hpp>

#include "utilities.h"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    CaptureThread(QString videoPath, QMutex *lock);
    ~CaptureThread();
    void setRunning(bool run) { running = run; };
    void takePhoto() { takingPhoto = true; }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void photoTaken(QString name);

private:
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *dataLock;
    cv::Mat frame;
    // take photos
    bool takingPhoto;
    void takePhoto(cv::Mat &frame);
};

#endif // CAPTURE_THREAD_H
