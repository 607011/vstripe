/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QtGlobal>

#include "videowidget.h"


VideoWidget::VideoWidget(QWidget* parent) : QWidget(parent)
{
    setAcceptDrops(true);
    setBaseSize(480, 270);
    setMinimumSize(384, 216);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mStripeWidth = 1;
    mDragging = false;
    mVerticalStripe = true;
}


bool VideoWidget::stripeIsFixed(void) const
{
    return mVerticalStripe? (mStripeX >= 0) : (mStripeY >= 0);
}


int VideoWidget::stripePos(void) const
{
    return mVerticalStripe? mStripeX * mImage.width()  / mDestRect.width()
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
    painter.setRenderHint(QPainter::Antialiasing);

    // draw background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(0, 0, width(), height());

    // draw image
    painter.drawImage(mDestRect, mImage);

    // draw stripe
    if (mVerticalStripe) {
        if (mStripeX >= 0) {
            painter.setBrush(QColor(0xff, 0x00, 0x00, 0xcc));
            painter.drawRect(mStripeX + mDestRect.x(), mDestRect.y(), mStripeWidth, mDestRect.height());
        }
        else {
            painter.setPen(QColor(0x00, 0x00, 0xff, 0x99));
            painter.setBrush(QColor(0x00, 0x00, 0xff, 0x66));
            QPainterPath path;
            path.moveTo(width()/2+width()/20, height()/2);
            path.lineTo(width()/2-width()/20, height()/2-height()/10);
            path.lineTo(width()/2-width()/20, height()/2+height()/10);
            path.lineTo(width()/2+width()/20, height()/2);
            painter.drawPath(path);
        }
    }
    else {
        if (mStripeY >= 0) {
            painter.setBrush(QColor(0xff, 0x00, 0x00, 0xcc));
            painter.drawRect(mDestRect.x(), mStripeY + mDestRect.y(), mDestRect.width(), mStripeWidth);
        }
        else {
            painter.setPen(QColor(0x00, 0x00, 0xff, 0x99));
            painter.setBrush(QColor(0x00, 0x00, 0xff, 0x66));
            QPainterPath path;
            path.moveTo(width()/2,            height()/2+height()/20);
            path.lineTo(width()/2-width()/20, height()/2-height()/20);
            path.lineTo(width()/2+width()/20, height()/2-height()/20);
            path.lineTo(width()/2,            height()/2+height()/20);
            painter.drawPath(path);
        }
    }
}


void VideoWidget::keyPressEvent(QKeyEvent* event)
{
    if (mDragging) {
        mVerticalStripe = ((event->modifiers() & Qt::ControlModifier) == 0);
        update();
    }
}


void VideoWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (mDragging) {
        mVerticalStripe = ((event->modifiers() & Qt::ControlModifier) == 0);
        update();
    }
}


void VideoWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mDragging) {
        mVerticalStripe = ((event->modifiers() & Qt::ControlModifier) == 0);
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
    mDragging = (event->button() == Qt::LeftButton);
    if (mDragging)
        setFocus(Qt::MouseFocusReason);
}


void VideoWidget::dragEnterEvent(QDragEnterEvent* event)
{
    // only accept local files
    if (event->mimeData()->hasUrls()) {
        bool accepted = true;
        foreach (const QUrl& url, event->mimeData()->urls()) {
            accepted &= !url.toLocalFile().isEmpty();
            if (!accepted)
                break;
        }
        if (accepted)
            event->acceptProposedAction();
    }
}


void VideoWidget::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls())
        return;
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    emit fileDropped(urls.first().toLocalFile());
}
