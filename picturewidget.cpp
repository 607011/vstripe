/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include "picturewidget.h"


PictureWidget::PictureWidget(QWidget* parent) : QWidget(parent)
{
    setStyleSheet("background: #444");
}


const QString PictureWidget::winTitle = QObject::tr("VStripe - Picture Preview");

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


void PictureWidget::resizeEvent(QResizeEvent*)
{
    setWindowTitle(winTitle + QString(" - %1 x %2").arg(width()).arg(height()));
}
