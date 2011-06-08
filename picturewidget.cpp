/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include "picturewidget.h"


PictureWidget::PictureWidget(QWidget* parent) : QWidget(parent), mBrightnessData(NULL), mAvgBrightness(-1)
{
    setStyleSheet("background: #444");
}


void PictureWidget::setPicture(const QImage& img)
{
    mImage = img;
    update();
}


void PictureWidget::setBrightnessData(const BrightnessData* d, qreal avgBrightness)
{
    mBrightnessData = d;
    mAvgBrightness = avgBrightness;
    update();
}


void PictureWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), mImage);
    if (mBrightnessData && !mBrightnessData->isEmpty()) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(0xff, 0x30, 0x30, 0xa0), 1.65));
        painter.setBrush(Qt::NoBrush);
        const int y0 = mImage.height();
        QPainterPath path;
        path.moveTo(0, y0);
        for (int i = 0; i < mBrightnessData->size(); ++i)
            path.lineTo(i, y0-(*mBrightnessData)[i]);
        painter.drawPath(path);
        if (mAvgBrightness >= 0) {
            painter.setPen(QPen(QColor(0x30, 0xff, 0x30, 0xa0), 1.65));
            painter.drawLine(QLineF(0, y0-mAvgBrightness, mImage.width(), y0-mAvgBrightness));
        }
    }
}
