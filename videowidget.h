/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * $Id$
 */

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QFrame>
#include <QImage>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>

extern "C" {
#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>
}

class VideoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit VideoWidget(QFrame* parent = 0);
    ~VideoWidget();
    void setFrame(AVFrame* pFrame);

    QSize minimumSizeHint(void) const { return QSize(384, 216); }

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QImage image;
    qreal windowAspectRatio;
    qreal frameAspectRatio;
    QRect destRect;

    void calcDestRect(void);

signals:

public slots:
    void setFrameSize(int w, int h);
};

#endif // VIDEOWIDGET_H
