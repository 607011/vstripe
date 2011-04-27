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

    qreal prevFrameNumber = mFrameNumber;
    int frameCount = 0;
    int percent = 0, prevPercent = 0;
    mDecoder.seekFrame(mFrameNumber);
    while (!mAbort && frameCount < mMaxFrameCount) {
        int skip = (int)mFrameNumber - (int)prevFrameNumber;
        if (skip > 0) {
            mDecoder.seekNextFrame(skip);
            prevFrameNumber = mFrameNumber;
        }
        QImage img;
        int effFrameNum, effFrameTime;
        mDecoder.getFrame(img, &effFrameNum, &effFrameTime);
        emit frameReady(img, frameCount, effFrameNum, effFrameTime);
        ++frameCount;
        mFrameNumber += mFrameSkip;
        percent = 100 * frameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
