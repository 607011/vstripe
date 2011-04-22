/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include "markableslider.h"

MarkableSlider::MarkableSlider(const Project* project, QSlider* parent) : QSlider(Qt::Horizontal, parent), mProject(project)
{
    setMinimum(0);
}


void MarkableSlider::paintEvent(QPaintEvent* event)
{
    Q_ASSERT(mProject != NULL);

    QSlider::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal scale = (qreal)width() / (qreal)maximum();

    // draw A marker
    if (mProject->markA() >= 0) {
        QPainterPath pathA;
        qreal xa = mProject->markA() * scale;
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
    if (mProject->markB() >= 0) {
        QPainterPath pathB;
        qreal xb = mProject->markB() * scale;
        pathB.moveTo(xb, 0);
        pathB.lineTo(xb-height()/2, 0);
        pathB.lineTo(xb, height()/2);
        pathB.lineTo(xb-height()/2, height());
        pathB.lineTo(xb, height());
        pathB.lineTo(xb, 0);
        painter.setBrush(QColor(0x99, 0, 0, 0xc0));
        painter.drawPath(pathB);
    }

    if (mProject->marks().count() > 0) {
        QPainterPath path;
        foreach (Project::mark_type i, mProject->marks()) {
            qreal x = i.first * scale;
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
