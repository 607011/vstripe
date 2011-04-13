/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * $Id$
 */

#ifndef VIDEOREADERTHREAD_H
#define VIDEOREADERTHREAD_H

extern "C" {
#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>
}

#include <QThread>
#include <QString>

#include "videowidget.h"

class VideoReaderThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoReaderThread(VideoWidget* videoWidget, QObject* parent = 0);
    ~VideoReaderThread();

    void startReading(const QString& videoFileName);
    void stopReading(void);

signals:

public slots:

protected:
    void run(void);

private:
    QString mFileName;
    VideoWidget* mVideoWidget;
    bool abort;
};

#endif // VIDEOREADERTHREAD_H
