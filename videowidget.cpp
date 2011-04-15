/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QDebug>
#include <QtGlobal>

#include "videowidget.h"


VideoWidget::VideoWidget(QWidget* parent) : QWidget(parent)
{
    setBaseSize(480, 270);
    setMinimumSize(384, 216);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setSizeIncrement(16, 9);
    mStripeWidth = 1;
    mDragging = false;
    setFocus();
}


void VideoWidget::calcDestRect(void) {
    if (mWindowAspectRatio < mFrameAspectRatio) {
        const int h = (int)(width() / mFrameAspectRatio);
        mDestRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = (int)(height() * mFrameAspectRatio);
        mDestRect = QRect((width()-w)/2, 0, w, height());
    }
}


void VideoWidget::setFrameSize(const QSize& sz) {
    mFrameAspectRatio = (qreal)sz.width() / (qreal)sz.height();
    calcDestRect();
    update();
}


void VideoWidget::setStripeWidth(int stripeWidth)
{
    mStripeWidth = stripeWidth;
    update();
}


int VideoWidget::stripePos(void) const
{
    int x = mStripeX * mImage.width() / mDestRect.width();
    qDebug() << "stripePos() = " << x;
    return x;
}


void VideoWidget::setFrame(QImage img)
{
    mImage = img;
    update();
}


void VideoWidget::resizeEvent(QResizeEvent* e)
{
    mWindowAspectRatio = (qreal)e->size().width() / (qreal)e->size().height();
    calcDestRect();
    update();
}


void VideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // draw background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(0, 0, width(), height());

    // draw image
    painter.drawImage(mDestRect, mImage);

    // draw stripe
    if (mStripeX >= 0) {
        painter.setBrush(QColor(qRgba(0xff, 0x00, 0x00, 0x7f)));
        painter.drawRect(mStripeX + mDestRect.x(), mDestRect.y(), mStripeWidth, mDestRect.height());
    }
}


void VideoWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mDragging) {
        mStripeX = event->x() - mDestRect.x();
        if (mStripeX >= mDestRect.width())
            mStripeX = mDestRect.width() - 1;
        update();
    }
}


void VideoWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        mDragging = true;
        mDragStartPos = event->pos();
    }
}


void VideoWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && mDragging)  {

    }
}
