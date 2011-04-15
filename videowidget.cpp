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
    mVerticalStripe = true;
}


int VideoWidget::stripePos(void) const
{
    return mVerticalStripe
            ? mStripeX * mImage.width() / mDestRect.width()
            : mStripeY * mImage.height() / mDestRect.height();
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
    if (mVerticalStripe) {
        if (mStripeX >= 0) {
            painter.setBrush(QColor(qRgb(0xff, 0x00, 0x00)));
            painter.drawRect(mStripeX + mDestRect.x(), mDestRect.y(), mStripeWidth, mDestRect.height());
        }
    }
    else {
        if (mStripeY >= 0) {
            painter.setBrush(QColor(qRgb(0xff, 0x00, 0x00)));
            painter.drawRect(mDestRect.x(), mStripeY + mDestRect.y(), mDestRect.width(), mStripeWidth);
        }
    }
}


void VideoWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mDragging) {
        mVerticalStripe = (event->modifiers() & Qt::ControlModifier) == 0;
        if (mVerticalStripe) {
            mStripeX = event->x() - mDestRect.x();
            if (mStripeX >= mDestRect.width())
                mStripeX = -1;
        }
        else {
            mStripeY = event->y() - mDestRect.y();
            if (mStripeY >= mDestRect.height())
                mStripeY = -1;
        }
        update();
    }
}


void VideoWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        mVerticalStripe = (event->modifiers() & Qt::ControlModifier) == 0;
        mDragging = true;
    }
}


void VideoWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && mDragging)  {

    }
}
