/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>

#include "webcam.h"

#include "opencv2/opencv.hpp"


Webcam::Webcam(QObject* parent) :
        IAbstractVideoDecoder(parent),
        mCamera(NULL),
        mFrameNumber(0),
        mFrameTime(0)
{
    /* ... */
}


Webcam::~Webcam()
{
    close();
}


bool Webcam::open(int deviceId)
{
    close();
    mCamera = new cv::VideoCapture(deviceId);
    if (mCamera->isOpened()) {
        cv::Mat frame;
        *mCamera >> frame;
        mFrameSize = QSize((int)mCamera->get(CV_CAP_PROP_FRAME_WIDTH), (int)mCamera->get(CV_CAP_PROP_FRAME_HEIGHT));
        return true;
    }
    return false;
}


bool Webcam::open(const char*)
{
    return false;
}


void Webcam::close(void)
{
    if (mCamera)
        delete mCamera;
    mCamera = NULL;
}


QSize Webcam::frameSize(void) const
{
    return mFrameSize;
}


int Webcam::getVideoLengthMs(void)
{
    return -1;
}


QString Webcam::codecInfo(void) const
{
    return QString();
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
    Q_ASSERT(mCamera != NULL);

    cv::Mat frame;
    *mCamera >> frame;
    const int w = frame.cols;
    const int h = frame.rows;
    mLastFrame = QImage(w, h, QImage::Format_RGB888);
    for (int y = 0; y < h; ++y)
        memcpy(mLastFrame.scanLine(y), frame.ptr(y), 3*w);
    mLastFrame = mLastFrame.rgbSwapped();
    ++mFrameNumber;
    ++mFrameTime; // XXX
    return true;
}


bool Webcam::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int*, int*)
{
    seekNextFrame();
    img = mLastFrame;
    if (effectiveframenumber)
        *effectiveframenumber = mFrameNumber;
    if (effectiveframetime)
        *effectiveframetime = mFrameTime;
    return true;
}
