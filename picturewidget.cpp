/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include "picturewidget.h"


PictureWidget::PictureWidget(QWidget* parent) :
        QWidget(parent),
        mBrightnessData(NULL),
        mRedData(NULL),
        mGreenData(NULL),
        mBlueData(NULL),
        mAvgBrightness(-1),
        mAvgRed(-1),
        mAvgGreen(-1),
        mAvgBlue(-1)
{
    setStyleSheet("background: #444");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void PictureWidget::setPicture(const QImage& img)
{
    mImage = img;
    update();
}


void PictureWidget::setBrightnessData(
        const BrightnessData* brightness,
        const BrightnessData* red,
        const BrightnessData* green,
        const BrightnessData* blue,
        qreal avgBrightness,
        qreal avgRed,
        qreal avgGreen,
        qreal avgBlue)
{
    mBrightnessData = brightness;
    mRedData = red;
    mGreenData = green;
    mBlueData = blue;
    mAvgBrightness = avgBrightness;
    mAvgRed = avgRed;
    mAvgGreen = avgGreen;
    mAvgBlue = avgBlue;
    update();
}


void PictureWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), mImage);
    if (mBrightnessData && !mBrightnessData->isEmpty()) {
        const int y0l = mImage.height();
        const int y0r = mImage.height()/4;
        const int y0g = mImage.height()*2/4;
        const int y0b = mImage.height()*3/4;
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        if (mAvgBrightness >= 0) {
            painter.setPen(QPen(QColor(0xff, 0xff, 0xff, 0xa0), 1.65));
            painter.drawLine(QLineF(0, y0l-mAvgBrightness, mImage.width(), y0l-mAvgBrightness));
        }
        if (mAvgRed >= 0) {
            painter.setPen(QPen(QColor(0xff, 0x00, 0x00, 0xa0), 1.65));
            painter.drawLine(QLineF(0, y0r-mAvgRed, mImage.width(), y0r-mAvgRed));
        }
        if (mAvgGreen >= 0) {
            painter.setPen(QPen(QColor(0x00, 0xff, 0x00, 0xa0), 1.65));
            painter.drawLine(QLineF(0, y0g-mAvgGreen, mImage.width(), y0g-mAvgGreen));
        }
        if (mAvgBlue >= 0) {
            painter.setPen(QPen(QColor(0x00, 0x00, 0xff, 0xa0), 1.65));
            painter.drawLine(QLineF(0, y0b-mAvgBlue, mImage.width(), y0b-mAvgBlue));
        }
        painter.setPen(QPen(QColor(0xcc, 0xcc, 0xcc, 0xa0), 1.65));
        QPainterPath path;
        path.moveTo(0, y0l);
        for (int i = 0; i < mBrightnessData->size(); ++i)
            path.lineTo(i, y0l-(*mBrightnessData)[i]);
        path.moveTo(0, y0r);
        for (int i = 0; i < mRedData->size(); ++i)
            path.lineTo(i, y0r-(*mRedData)[i]);
        path.moveTo(0, y0g);
        for (int i = 0; i < mGreenData->size(); ++i)
            path.lineTo(i, y0g-(*mGreenData)[i]);
        path.moveTo(0, y0b);
        for (int i = 0; i < mBlueData->size(); ++i)
            path.lineTo(i, y0b-(*mBlueData)[i]);
        painter.drawPath(path);
    }
}
