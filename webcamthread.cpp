/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>

#include "webcamthread.h"
#include "webcam.h"

WebcamThread::WebcamThread(Webcam* webcam, QObject* parent) :
    QThread(parent), mWebcam(webcam), mAbort(false)
{
}


WebcamThread::~WebcamThread()
{
    stopReading();
}


void WebcamThread::startReading(void)
{
    Q_ASSERT(mWebcam != NULL);

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
    Q_ASSERT(mWebcam != NULL);

    QImage frame;
    int framenumber = 0;
    while (!mAbort) {
        mWebcam->getFrame(frame, &framenumber);
        emit frameReady(frame);
    }
    qDebug() << "STOPPING WebcamThread::run() ...";
}
