/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>

#include "webcamthread.h"
#include "webcam.h"

WebcamThread::WebcamThread(QObject* parent) :
    QThread(parent), mWebcam(NULL), mAbort(false)
{
}


WebcamThread::~WebcamThread()
{
    stopReading();
}


void WebcamThread::setDecoder(Webcam* webcam)
{
    mWebcam = webcam;
}


void WebcamThread::startReading(void)
{
    stopReading();
    mAbort = false;
    start();
}


void WebcamThread::stopReading(void)
{
    if (isRunning()) {
        mAbort = true;
        wait();
    }
}


void WebcamThread::run(void)
{
    QImage frame;
    while (!mAbort) {
        mWebcam->getFrame(frame);
        emit frameReady(frame);
    }
}
