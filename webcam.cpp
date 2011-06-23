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
    mCamera = cvCaptureFromCAM(deviceId);
    mFrameSize = QSize((int)cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_WIDTH), (int)cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_HEIGHT));
    qDebug() << "CV_CAP_PROP_POS_FRAMES =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_POS_FRAMES);
    qDebug() << "CV_CAP_PROP_FRAME_WIDTH =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_WIDTH);
    qDebug() << "CV_CAP_PROP_FRAME_HEIGHT  =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_HEIGHT);
    qDebug() << "CV_CAP_PROP_FPS =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_FPS);
    qDebug() << "CV_CAP_PROP_BRIGHTNESS =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_BRIGHTNESS);
    qDebug() << "CV_CAP_PROP_CONTRAST =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_CONTRAST);
    qDebug() << "CV_CAP_PROP_SATURATION =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_SATURATION);
    qDebug() << "CV_CAP_PROP_HUE =" << cvGetCaptureProperty(mCamera, CV_CAP_PROP_HUE);
    IplImage* img = cvQueryFrame(mCamera);
    qDebug() << "dataOrder =" << img->dataOrder;
    qDebug() << "nChannels =" << img->nChannels;
    qDebug() << "depth =" << img->depth;
    cvReleaseImage(&img);
    return mCamera != NULL;
}


void Webcam::close(void)
{
    if (mCamera)
        cvReleaseCapture(&mCamera);
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

    IplImage* img = cvQueryFrame(mCamera);
    const int w = img->width;
    const int h = img->height;
    mLastFrame = QImage(w, h, QImage::Format_RGB888);
    switch (img->depth) { // http://snipplr.com/view/40277/iplimage-to-qimage--qimage-to-iplimage/
    case IPL_DEPTH_8U:
        {
            const quint8* src = reinterpret_cast<const quint8*>(img->imageData);
            if (img->nChannels == 1 || img->nChannels == 3) {
                for (int y = 0; y < h; ++y) {
                    memcpy(mLastFrame.scanLine(y), src, 3*w);
                    src += img->widthStep;
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
        qWarning() << "UNSUPPORTED IplImage depth:" << img->depth;
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
