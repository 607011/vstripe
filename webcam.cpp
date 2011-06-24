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


bool Webcam::open(const char*)
{
    return false;
}


bool Webcam::open(int deviceId)
{
    mCamera = new cv::VideoCapture(deviceId);
    if (mCamera->isOpened()) {
        cv::Mat frame;
        *mCamera >> frame;
        mFrameSize = QSize((int)mCamera->get(CV_CAP_PROP_FRAME_WIDTH), (int)mCamera->get(CV_CAP_PROP_FRAME_HEIGHT));
        qDebug() << "webcam frame size =" << mFrameSize;
        qDebug() << "CV_CAP_PROP_POS_FRAMES =" << mCamera->get(CV_CAP_PROP_POS_FRAMES);
        qDebug() << "CV_CAP_PROP_FPS =" << mCamera->get(CV_CAP_PROP_FPS);
        qDebug() << "CV_CAP_PROP_BRIGHTNESS =" << mCamera->get(CV_CAP_PROP_BRIGHTNESS);
        qDebug() << "CV_CAP_PROP_CONTRAST =" << mCamera->get(CV_CAP_PROP_CONTRAST);
        qDebug() << "CV_CAP_PROP_SATURATION =" << mCamera->get(CV_CAP_PROP_SATURATION);
        qDebug() << "CV_CAP_PROP_HUE =" << mCamera->get(CV_CAP_PROP_HUE);
        qDebug() << "depth =" << frame.depth();
        qDebug() << "nChannels =" << frame.channels();
        qDebug() << "width =" << frame.cols;
        qDebug() << "height =" << frame.rows;
        return true;
    }
    return false;
//    mCamera = cvCaptureFromCAM(deviceId);
//    mFrameSize = QSize((int)cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_WIDTH), (int)cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_HEIGHT));
//    qDebug() << "webcam frame size =" << mFrameSize;
//    qDebug() << "CV_CAP_PROP_POS_FRAMES =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_POS_FRAMES);
//    qDebug() << "CV_CAP_PROP_FPS =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_FPS);
//    qDebug() << "CV_CAP_PROP_BRIGHTNESS =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_BRIGHTNESS);
//    qDebug() << "CV_CAP_PROP_CONTRAST =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_CONTRAST);
//    qDebug() << "CV_CAP_PROP_SATURATION =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_SATURATION);
//    qDebug() << "CV_CAP_PROP_HUE =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_HUE);
//    IplImage* img = cvQueryFrame(mCamera);
//    qDebug() << "dataOrder =" << img->dataOrder;
//    qDebug() << "nChannels =" << img->nChannels;
//    qDebug() << "depth =" << img->depth;
//    qDebug() << "width =" << img->width;
//    qDebug() << "height =" << img->height;
//    return mCamera != NULL;
}


void Webcam::close(void)
{
    if (mCamera)
        delete mCamera;
//    if (mCamera)
//        cvReleaseCapture(&mCamera);
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
    switch (frame.depth()) { // http://snipplr.com/view/40277/iplimage-to-qimage--qimage-to-iplimage/
    case IPL_DEPTH_8U:
        {
            const quint8* src = reinterpret_cast<const quint8*>(frame.data);
            if (frame.channels() == 1 || frame.channels() == 3) {
                for (int y = 0; y < h; ++y) {
                    memcpy(mLastFrame.scanLine(y), src, 3*w);
                    src += frame.cols;
                }
            }
        }
        break;
    case IPL_DEPTH_16U:
        break;
    case IPL_DEPTH_32F:
        break;
    case IPL_DEPTH_64F:
        break;
    default:
        qWarning() << "UNSUPPORTED IplImage depth:" << frame.depth();
        return false;
    }
    ++mFrameNumber;
    ++mFrameTime; // XXX
    return true;
}


bool Webcam::getFrame(QImage& img, int* effectiveframenumber, int* effectiveframetime, int*, int*) const
{
    img = mLastFrame;
    *effectiveframenumber = mFrameNumber;
    *effectiveframetime = mFrameTime;
    return true;
}
