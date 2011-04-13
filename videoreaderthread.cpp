/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QMutexLocker>
#include "videoreaderthread.h"

VideoReaderThread::VideoReaderThread(VideoWidget* videoWidget, QObject* parent)
    : QThread(parent), mVideoWidget(videoWidget)
{
    mSkip = 1;
}


VideoReaderThread::~VideoReaderThread()
{
    stopReading();
}


void VideoReaderThread::setFile(QString videoFileName)
{
    mDecoder.openFile(videoFileName.toLatin1().constData());
    qDebug() << "Video length: " << 1e-3 * mDecoder.getVideoLengthMs() / 3600.0 << " hours";
}


void VideoReaderThread::startReading(int numFrames, QVector<QImage>* images, int skip)
{
    stopReading();
    mSkip = skip;
    mImages = images;
    mImages->clear();
    mVideoWidget->setFrameSize(mDecoder.frameSize());
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
        mVideoWidget->setFrame(img); // TODO: Kann einen SegFault verursachen, wenn der Thread ein QImage beschreibt, das anderswo gelesen wird
        mImages->append(img);
        ++mFrameCount;
        percent = 100 * mFrameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
