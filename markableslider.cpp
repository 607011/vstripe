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
    foreach (Project::mark_type i, mProject->marks()) {
        if (i.frame == Project::INVALID_FRAME) {
            qDebug() << "INVALID_FRAME detected: " << i.id << ", " << i.name;
            continue;
        }
        QPainterPath path;
        qreal x = i.frame * scale;
        if (i.id == Project::ID_A) {
            path.moveTo(x, 0);
            path.lineTo(x+height()/2, 0);
            path.lineTo(x, height()/2);
            path.lineTo(x+height()/2, height());
            path.lineTo(x, height());
            path.lineTo(x, 0);
            painter.setBrush(QColor(0, 0x99, 0, 0xc0));
        }
        else if (i.id == Project::ID_B) {
            path.moveTo(x, 0);
            path.lineTo(x-height()/2, 0);
            path.lineTo(x, height()/2);
            path.lineTo(x-height()/2, height());
            path.lineTo(x, height());
            path.lineTo(x, 0);
            painter.setBrush(QColor(0x99, 0, 0, 0xc0));
        }
        else {
            path.moveTo(x-height()/4, 0);
            path.lineTo(x+height()/4, 0);
            path.lineTo(x-height()/4, height());
            path.lineTo(x+height()/4, height());
            path.lineTo(x-height()/4, 0);
            painter.setBrush(QColor(0x66, 0x66, 0x66, 0xc0));
        }
        painter.drawPath(path);
    }
}
