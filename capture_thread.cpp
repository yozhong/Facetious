#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), dataLock(lock)
{
    takingPhoto = false;
}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), dataLock(lock)
{
    takingPhoto = false;
}

CaptureThread::~CaptureThread() {}

void CaptureThread::run()
{
    running = true;
    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    while(running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) {
            break;
        }

        if(takingPhoto) {
            takePhoto(tmpFrame);
        }

        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);
        dataLock->lock();
        frame = tmpFrame;
        dataLock->unlock();
        emit frameCaptured(&frame);
    }

    cap.release();
    running = false;
}

void CaptureThread::takePhoto(cv::Mat &frame)
{
    QString photoName = Utilities::newPhotoName();
    QString photoPath = Utilities::getPhotoPath(photoName, "jpg");

    cv::imwrite(photoPath.toStdString(), frame);
    emit photoTaken(photoName);
    takingPhoto = false;
}
