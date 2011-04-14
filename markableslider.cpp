/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include "markableslider.h"

MarkableSlider::MarkableSlider(QSlider* parent) : QSlider(Qt::Horizontal, parent), mA(-1), mB(-1)
{
    setMinimum(0);
}


void MarkableSlider::setA(int a)
{
    mA = a;
    update();
}


void MarkableSlider::setB(int b)
{
    mB = b;
    update();
}


void MarkableSlider::paintEvent(QPaintEvent* event)
{
    QSlider::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    // draw A marker
    if (mA >= 0) {
        QPainterPath pathA;
        qreal xa = (qreal)mA * (qreal)width() /(qreal)maximum();
        pathA.moveTo(xa, 0);
        pathA.lineTo(xa, height());
        pathA.lineTo(xa+height()/2, height()/2);
        pathA.lineTo(xa, 0);
        painter.setBrush(QColor(0, 0xcc, 0));
        painter.drawPath(pathA);
    }

    // draw B marker
    if (mB >= 0) {
        QPainterPath pathB;
        qreal xb = (qreal)mB * (qreal)width() /(qreal)maximum();
        pathB.moveTo(xb, 0);
        pathB.lineTo(xb, height());
        pathB.lineTo(xb-height()/2, height()/2);
        pathB.lineTo(xb, 0);
        painter.setBrush(QColor(0xcc, 0, 0));
        painter.drawPath(pathB);
    }
}
