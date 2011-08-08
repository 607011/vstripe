/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>

#include "webcam.h"

#include "opencv/cv.h"
#include "opencv/cv.hpp"
#include "opencv/highgui.h"


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
        return !mFrameSize.isNull();
    }
    return false;
}


void Webcam::close(void)
{
    if (mCamera)
        delete mCamera;
    mCamera = NULL;
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
    mLastFrame = mLastFrame.rgbSwapped().mirrored(true, false);
    ++mFrameNumber;
    ++mFrameTime; // XXX
    return true;
}


bool Webcam::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int*, int*)
{
    seekNextFrame(0);
    img = mLastFrame;
    if (effectiveframenumber)
        *effectiveframenumber = mFrameNumber;
    if (effectiveframetime)
        *effectiveframetime = mFrameTime;
    return true;
}


void Webcam::setSize(const QSize& sz)
{
    Q_ASSERT(sz.isValid());
    Q_ASSERT(mCamera != NULL);

    mCamera->set(CV_CAP_PROP_FRAME_WIDTH, sz.width());
    mCamera->set(CV_CAP_PROP_FRAME_HEIGHT, sz.height());
}
