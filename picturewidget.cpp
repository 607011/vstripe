/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include "picturewidget.h"


PictureWidget::PictureWidget(QWidget* parent) : QWidget(parent)
{
    setStyleSheet("background: #444");
}


void PictureWidget::setPicture(const QImage& img)
{
    mImage = img;
    update();
}


void PictureWidget::setBrightnessData(BrightnessData* d)
{

}


void PictureWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), mImage);
}
