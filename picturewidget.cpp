/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include "picturewidget.h"


const QString PictureWidget::WinTitle = QObject::tr("VStripe - Picture Preview");


PictureWidget::PictureWidget(QWidget* parent) : QWidget(parent)
{
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
    painter.drawImage(QPoint(0, 0), mImage);
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
    setWindowTitle(QString("%1 - %2x%3").arg(WinTitle).arg(width()).arg(height()));
}
