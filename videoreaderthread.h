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
#include <QMutex>

#include "videowidget.h"
#include "videodecoder.h"

class VideoReaderThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoReaderThread(VideoWidget* videoWidget, QObject* parent = NULL);
    ~VideoReaderThread();

    void setFile(QString videoFileName);
    void startReading(int numFrames, QVector<QImage>* images, int skip = 1);
    void stopReading(void);

    VideoDecoder* decoder(void) { return &mDecoder; }

signals:

public slots:

protected:
    void run(void);

private:
    VideoWidget* mVideoWidget;
    VideoDecoder mDecoder;
    bool mAbort;
    QVector<QImage>* mImages;
    int mMaxFrameCount;
    int mFrameCount;
    int mSkip;
};

#endif // VIDEOREADERTHREAD_H
