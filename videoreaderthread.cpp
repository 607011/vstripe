/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
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


void VideoReaderThread::startReading(int firstFrameNumber, int nFrames, qreal frameDelta)
{
    Q_ASSERT(firstFrameNumber != Project::INVALID_FRAME);
    Q_ASSERT(nFrames > 0);
    Q_ASSERT(frameDelta > 0);

    stopReading();
    mFrameNumber = qreal(firstFrameNumber);
    mMaxFrameCount = nFrames;
    mFrameDelta = frameDelta;
    mAbort = false;
    start();
}


void VideoReaderThread::stopReading(void)
{
    if (isRunning()) {
        mAbort = true;
        wait();
    }
}


void VideoReaderThread::run(void)
{
    Q_ASSERT(mFrameNumber != Project::INVALID_FRAME);

    qreal prevFrameNumber = mFrameNumber;
    int frameCount = 0;
    int percent = 0, prevPercent = 0;
    QImage img;
    int effFrameNum, effFrameTime;
    mDecoder.seekFrame(mFrameNumber);
    mDecoder.getFrame(img, &effFrameNum, &effFrameTime);
    while (!mAbort && frameCount < mMaxFrameCount) {
        int skip = int(mFrameNumber) - int(prevFrameNumber);
        if (skip > 0) {
            mDecoder.seekNextFrame(skip);
            prevFrameNumber = mFrameNumber;
            mDecoder.getFrame(img, &effFrameNum, &effFrameTime);
        }
        emit frameReady(img, frameCount, effFrameNum, effFrameTime);
        ++frameCount;
        mFrameNumber += mFrameDelta;
        percent = 100 * frameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
