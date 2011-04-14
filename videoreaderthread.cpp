/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QMutexLocker>
#include "videoreaderthread.h"

VideoReaderThread::VideoReaderThread(QObject* parent)
    : QThread(parent), mSkip(1)
{
}


VideoReaderThread::~VideoReaderThread()
{
    stopReading();
}


void VideoReaderThread::setFile(QString videoFileName)
{
    mDecoder.openFile(videoFileName.toLatin1().constData());
}


void VideoReaderThread::startReading(int numFrames, int skip)
{
    stopReading();
    mSkip = skip;
    mMaxFrameCount = numFrames;
    mFrameCount = 0;
    mAbort = false;
    start();
}


void VideoReaderThread::stopReading(void)
{
    mAbort = true;
    wait();
}


void VideoReaderThread::run(void)
{
    int percent = 0, prevPercent = 0;
    while (!mAbort && mFrameCount < mMaxFrameCount) {
        QImage img;
        mDecoder.seekNextFrame(mSkip);
        mDecoder.getFrame(img);
        emit frameReady(img, mFrameCount);
        emit frameReady(img);
        ++mFrameCount;
        percent = 100 * mFrameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
