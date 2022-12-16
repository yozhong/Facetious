#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), dataLock(lock)
{

}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), dataLock(lock)
{

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

        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);
        dataLock->lock();
        frame = tmpFrame;
        dataLock->unlock();
        emit frameCaptured(&frame);
    }

    cap.release();
    running = false;
}
