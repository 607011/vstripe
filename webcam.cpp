#include "webcam.h"

Webcam::Webcam(QObject* parent) : IAbstractVideoDecoder(parent), mCamera(NULL)
{
}


Webcam::~Webcam()
{
    close();
}


bool Webcam::open(const char*)
{
    return false;
}


bool Webcam::open(int deviceId)
{
    mCamera = cvCaptureFromCAM(deviceId);
    return mCamera != NULL;
}


void Webcam::close(void)
{
    if (mCamera)
        cvReleaseCapture(&mCamera);
}


bool Webcam::seekFrame(qint64)
{
    return false;
}


bool Webcam::seekMs(int)
{
    return false;
}


bool Webcam::seekNextFrame(int)
{
    return false;
}


bool Webcam::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int* desiredframenumber, int* desiredframetime) const
{

    return true;
}
