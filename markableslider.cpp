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


void MarkableSlider::setMarks(const QVector<int>& marks)
{
    mMarks = marks;
    update();
}


void MarkableSlider::paintEvent(QPaintEvent* event)
{
    QSlider::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal scale = (qreal)width() / (qreal)maximum();

    // draw A marker
    if (mA >= 0) {
        QPainterPath pathA;
        qreal xa = mA * scale;
        pathA.moveTo(xa, 0);
        pathA.lineTo(xa+height()/2, 0);
        pathA.lineTo(xa, height()/2);
        pathA.lineTo(xa+height()/2, height());
        pathA.lineTo(xa, height());
        pathA.lineTo(xa, 0);
        painter.setBrush(QColor(0, 0x99, 0, 0xc0));
        painter.drawPath(pathA);
    }

    // draw B marker
    if (mB >= 0) {
        QPainterPath pathB;
        qreal xb = mB * scale;
        pathB.moveTo(xb, 0);
        pathB.lineTo(xb-height()/2, 0);
        pathB.lineTo(xb, height()/2);
        pathB.lineTo(xb-height()/2, height());
        pathB.lineTo(xb, height());
        pathB.lineTo(xb, 0);
        painter.setBrush(QColor(0x99, 0, 0, 0xc0));
        painter.drawPath(pathB);
    }

    if (mMarks.count() > 0) {
        QPainterPath path;
        foreach (int i, mMarks) {
            qreal x = i * scale;
            path.moveTo(x-height()/4, 0);
            path.lineTo(x+height()/4, 0);
            path.lineTo(x-height()/4, height());
            path.lineTo(x+height()/4, height());
            path.lineTo(x-height()/4, 0);
            painter.setBrush(QColor(0x66, 0x66, 0x66, 0xc0));
            painter.drawPath(path);
        }
    }
}
