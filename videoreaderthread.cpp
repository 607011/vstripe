/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QDebug>
#include <QMutexLocker>
#include "videoreaderthread.h"
#include "project.h"

VideoReaderThread::VideoReaderThread(QObject* parent)
    : QThread(parent), videoSource(NOTAVAILABLE), mHistogramEnabled(true)
{
    /* ... */
}


VideoReaderThread::~VideoReaderThread()
{
    stopReading();
}


bool VideoReaderThread::setFile(QString videoFileName)
{
    Q_ASSERT(!videoFileName.isNull());

    bool ok = mDecoder.openFile(videoFileName.toLatin1().constData());
    return ok;
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


void VideoReaderThread::setHistogramRegion(const QRect& rect)
{
    mHistogramRegion = rect;
}


void VideoReaderThread::calcHistogram(const QImage& src)
{
    const QRect histoRect = mHistogramRegion.isNull()? src.rect() : mHistogramRegion;
    // downscale image (region) for faster histogram generation
    QImage img(src.copy(histoRect).scaled(src.size().boundedTo(QSize(100, 100)), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    mHistogram.init(img.width() * img.height());
    for (int y = 0; y < img.height(); ++y) {
        const QRgb* d = reinterpret_cast<const QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x, ++d)
            mHistogram.add(QColor(*d).lightness());
    }
    mHistogram.postprocess();
}


void VideoReaderThread::run(void)
{
    Q_ASSERT(mFrameNumber != Project::INVALID_FRAME);

    qreal prevFrameNumber = mFrameNumber;
    int frameCount = 0;
    int percent = 0, prevPercent = 0;
    int effFrameNum, effFrameTime;
    mDecoder.seekFrame(mFrameNumber);
    while (!mAbort && frameCount < mMaxFrameCount) {
        int skip = int(mFrameNumber) - int(prevFrameNumber);
        if (skip > 0) {
            mDecoder.seekNextFrame(skip);
            prevFrameNumber = mFrameNumber;
        }
        mDecoder.getFrame(mCurrentFrame, &effFrameNum, &effFrameTime);
        if (skip > 0 && mHistogramEnabled)
            calcHistogram(mCurrentFrame);
        emit frameReady(mCurrentFrame, mHistogram, frameCount, effFrameNum, effFrameTime);
        ++frameCount;
        mFrameNumber += mFrameDelta;
        percent = 100 * frameCount / mMaxFrameCount;
        if (percent != prevPercent)
            emit percentReady(percent);
        prevPercent = percent;
    }
}
