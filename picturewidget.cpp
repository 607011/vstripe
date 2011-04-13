/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include "picturewidget.h"


PictureWidget::PictureWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(tr("VStripe - Picture Preview"));
    setStyleSheet("background: #444");
}


void PictureWidget::setPicture(const QImage& img)
{
    mImage = img;
    update();
}


void PictureWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(0, 0, mImage);
}


void PictureWidget::setVisible(bool visible)
{
    QWidget::setVisible(visible);
}


void PictureWidget::closeEvent(QCloseEvent*)
{
    emit visibilityChanged(false);
}
