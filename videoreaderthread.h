/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef VIDEOREADERTHREAD_H
#define VIDEOREADERTHREAD_H

#include <QThread>
#include <QString>
#include <QImage>
#include <QFile>
#include <QVector>

#include "videowidget.h"
#include "videodecoder.h"

class VideoReaderThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoReaderThread(QObject* parent = NULL);
    ~VideoReaderThread();

    void setFile(QString videoFileName);
    void startReading(int firstFrameNumber, int numFrames, qreal frameDelta = 1);
    void stopReading(void);

    VideoDecoder* decoder(void) { return &mDecoder; }

signals:
    void percentReady(int);
    void frameReady(QImage, int, int, int);

public slots:

protected:
    void run(void);

private:
    VideoDecoder mDecoder;
    volatile bool mAbort;
    int mMaxFrameCount;
    qreal mFrameNumber;
    qreal mFrameDelta;
};

#endif // VIDEOREADERTHREAD_H
