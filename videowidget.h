/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeSource>
#include <QKeyEvent>
#include <QUrl>

#include "ffmpeg.h"
#include "videoreaderthread.h"
#include "histogram.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = NULL);
    QSize minimumSizeHint(void) const { return QSize(384, 216); }
    QSize sizeHint(void) const { return QSize(1920, 1080); }
    int stripePos(void) const;
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }
    inline bool stripeIsFixed(void) const { return mStripePos >= 0; }
    void setStripePos(int);
    void setStripeOrientation(bool vertical);

public slots:
    void setFrameSize(const QSize&);
    void setStripeWidth(int);
    void setFrame(QImage, Histogram);
    void setHistogramEnabled(bool enabled = true);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    QImage mImage;
    bool mHistogramEnabled;
    Histogram mHistogram;
    qreal mWindowAspectRatio;
    qreal mFrameAspectRatio;
    QRect mDestRect;
    int mStripeWidth;
    bool mMouseButtonDown;
    bool mVerticalStripe;
    int mStripePos;
    QRect mHistoRegion;
    QPoint mMousePos;

    void calcDestRect(void);
    void calcStripePos(void);
    QPoint toRealPos(const QPoint&);
    QPoint toFramePos(const QPoint&);
    void constrainMousePos(void);

signals:
    void fileDropped(QString fileName);
    void stripeOrientationChanged(bool vertical);
    void stripePosChanged(int pos);
};

#endif // VIDEOWIDGET_H
