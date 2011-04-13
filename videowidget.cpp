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
}


VideoWidget::~VideoWidget() {
}


void VideoWidget::calcDestRect(void) {
    if (windowAspectRatio < frameAspectRatio) {
        const int h = (int)(width() / frameAspectRatio);
        destRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = (int)(height() * frameAspectRatio);
        destRect = QRect((width()-w)/2, 0, w, height());
    }
}


void VideoWidget::setFrameSize(const QSize& sz) {
    frameAspectRatio = (qreal)sz.width() / (qreal)sz.height();
    calcDestRect();
}


void VideoWidget::setFrame(const QImage& img)
{
    frameMutex.lock();
    image = img;
    frameMutex.unlock();
    update();
}


void VideoWidget::resizeEvent(QResizeEvent* e)
{
    windowAspectRatio = (qreal)e->size().width() / (qreal)e->size().height();
    calcDestRect();
}


void VideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(0, 0, width(), height());
    frameMutex.lock();
    painter.drawImage(destRect, image);
    frameMutex.unlock();
}
