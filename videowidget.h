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
#include "ffmpeg.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = NULL);
    QSize minimumSizeHint(void) const { return QSize(384, 216); }
    QSize sizeHint(void) const { return QSize(720, 576); }
    inline bool stripeFixed(void) const { return mStripeX >= 0; }
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }
    int stripePos(void) const;

public slots:
    void setFrameSize(const QSize&);
    void setStripeWidth(int);
    void setFrame(QImage);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    QImage mImage;
    qreal mWindowAspectRatio;
    qreal mFrameAspectRatio;
    QRect mDestRect;
    int mStripeWidth;
    bool mDragging;
    bool mVerticalStripe;
    int mStripeX;
    int mStripeY;

    void calcDestRect(void);


signals:

};

#endif // VIDEOWIDGET_H
