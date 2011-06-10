/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
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
    setTracking(false);
}


void MarkableSlider::paintEvent(QPaintEvent* event)
{
    Q_ASSERT(mProject != NULL);

    QSlider::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    qreal scale = qreal(width()) / qreal(maximum());
    foreach (Project::mark_type m, mProject->marks()) {
        if (m.frame == Project::INVALID_FRAME) {
            qWarning() << "MarkableSlider::paintEvent() reports: INVALID_FRAME detected, id =" << m.id << ", name=" << m.name;
            continue;
        }
        QPainterPath path;
        qreal x = m.frame * scale;
        if (m.id == Project::ID_A) {
            path.moveTo(x, 0);
            path.lineTo(x+height()/2, 0);
            path.lineTo(x, height()/2);
            path.lineTo(x+height()/2, height());
            path.lineTo(x, height());
            path.lineTo(x, 0);
            painter.setBrush(QColor(0, 0x99, 0, 0xc0));
        }
        else if (m.id == Project::ID_B) {
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
