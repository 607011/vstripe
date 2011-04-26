/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QMutexLocker>
#include "videoreaderthread.h"
#include "project.h"

VideoReaderThread::VideoReaderThread(QObject* parent)
    : QThread(parent)
{
}


VideoReaderThread::~VideoReaderThread()
{
    stopReading();
}


void VideoReaderThread::setFile(QString videoFileName)
{
    Q_ASSERT(!videoFileName.isNull());

    mDecoder.openFile(videoFileName.toLatin1().constData());
}


void VideoReaderThread::startReading(int firstFrameNumber, int nFrames, qreal skip)
{
    Q_ASSERT(firstFrameNumber != Project::INVALID_FRAME);
    Q_ASSERT(nFrames > 0);
    Q_ASSERT(skip > 0);

    stopReading();
    mFrameNumber = (qreal)firstFrameNumber;
    mMaxFrameCount = nFrames;
    mFrameSkip = skip;
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
    Q_ASSERT(mFrameNumber != Project::INVALID_FRAME);

    int percent = 0, prevPercent = 0;
    qreal prevFrameNumber = mFrameNumber;
    mDecoder.seekFrame((qint64)mFrameNumber);
    while (!mAbort && mFrameCount < mMaxFrameCount) {
        if ((mFrameNumber - prevFrameNumber) >= 1) {
            mDecoder.seekNextFrame((int)(mFrameNumber - prevFrameNumber));
            prevFrameNumber = mFrameNumber;
        }
        QImage img;
        mDecoder.getFrame(img);
        emit frameReady(img, mFrameCount);
        ++mFrameCount;
        mFrameNumber += mFrameSkip;
        percent = 100 * mFrameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
