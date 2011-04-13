/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QTime>
#include "videoreaderthread.h"

VideoReaderThread::VideoReaderThread(VideoWidget* videoWidget, QObject* parent)
    : QThread(parent), mVideoWidget(videoWidget)
{
    // ...
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


void VideoReaderThread::startReading(int numFrames, QVector<QImage>* images)
{
    mImages = images;
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
    while (!mAbort && mFrameCount < mMaxFrameCount) {
        mDecoder.seekNextFrame();
        QImage img;
        mDecoder.getFrame(img);
        mVideoWidget->setFrame(img);
        mImages->append(img);
        ++mFrameCount;
    }
}
