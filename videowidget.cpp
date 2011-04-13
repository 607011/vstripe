/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * $Id$
 */

#include <QPainter>
#include <QDebug>

#include "videowidget.h"

VideoWidget::VideoWidget(QFrame* parent) : QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Sunken);
    setBaseSize(480, 270);
    setMinimumSize(384, 216);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setSizeIncrement(16, 9);
}


VideoWidget::~VideoWidget() {
}


void VideoWidget::calcDestRect(void) {
    if (windowAspectRatio < frameAspectRatio) {
        const int h = (int)(width()/frameAspectRatio);
        destRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = (int)(height()*frameAspectRatio);
        destRect = QRect((width()-w)/2, 0, w, height());
    }
}


void VideoWidget::setFrameSize(int w, int h) {
    image = QImage(w, h, QImage::Format_RGB888);
    frameAspectRatio = (qreal)w / (qreal)h;
    calcDestRect();
}


void VideoWidget::setFrame(AVFrame* pFrame)
{
    for (int y = 0; y < image.height(); ++y) {
        quint8* scanLine = reinterpret_cast<quint8*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+0);
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+1);
            *scanLine++ = *(pFrame->data[0] + y*pFrame->linesize[0]+x+2);
        }
    }
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
    painter.drawImage(destRect, image);
}
