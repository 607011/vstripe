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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeSource>
#include <QKeyEvent>
#include <QUrl>

#include "ffmpeg.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = NULL);
    virtual QSize minimumSizeHint(void) const { return QSize(384, 216); }
    virtual QSize sizeHint(void) const { return QSize(1920, 1080); }
    int stripePos(void) const;
    inline bool stripeIsVertical(void) const { return mVerticalStripe; }
    inline bool stripeIsFixed(void) const { return mStripePos >= 0; }
    void setStripePos(int);
    void setStripeOrientation(bool vertical);

public slots:
    void setFrameSize(const QSize&);
    void setStripeWidth(int);
    void setFrame(QImage);

protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dropEvent(QDropEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);

private:
    QImage mImage;
    qreal mWindowAspectRatio;
    qreal mFrameAspectRatio;
    QRect mDestRect;
    int mStripeWidth;
    bool mDragging;
    bool mVerticalStripe;
    int mStripePos;
    QPoint mMousePos;

    void calcDestRect(void);
    void calcStripePos(void);


signals:
    void fileDropped(QString fileName);
    void stripeOrientationChanged(bool vertical);
    void stripePosChanged(int pos);
};

#endif // VIDEOWIDGET_H
