/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QFrame>
#include <QImage>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMutex>
#include <QVector>

#include "ffmpeg.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = NULL);
    ~VideoWidget();
    void setFrame(const QImage&);

    QSize minimumSizeHint(void) const { return QSize(384, 216); }

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QImage image;
    qreal windowAspectRatio;
    qreal frameAspectRatio;
    QRect destRect;
    QMutex frameMutex;
    int mStripeWidth;
    void calcDestRect(void);


signals:

public slots:
    void setFrameSize(const QSize&);
    void setStripeWidth(int);
};

#endif // VIDEOWIDGET_H
