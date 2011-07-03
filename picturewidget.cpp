/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QScrollArea>
#include <QScrollBar>
#include <QClipboard>
#include "picturewidget.h"

#include <qmath.h>

PictureWidget::PictureWidget(QWidget* parent) :
        QWidget(parent),
        mShowCurves(true),
        mBrightnessData(NULL),
        mRedData(NULL),
        mGreenData(NULL),
        mBlueData(NULL),
        mAvgBrightness(-1),
        mAvgRed(-1),
        mAvgGreen(-1),
        mAvgBlue(-1),
        mMinBrightness(-1),
        mMinRed(-1),
        mMinGreen(-1),
        mMinBlue(-1),
        mStripePos(-1),
        mStripeVertical(true),
        mDragging(false),
        mZoom(1.0),
        mScrollArea(NULL)
{
    // ...
}


void PictureWidget::copyImageToClipboard(void)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setImage(mImage);
}


void PictureWidget::showCurves(bool enabled)
{
    mShowCurves = enabled;
    update();
}


void PictureWidget::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_C && (e->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        copyImageToClipboard();
}


void PictureWidget::resizeEvent(QResizeEvent*)
{
    // ...
}


void PictureWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
        mDragStartPos = event->pos() - pos();
        mDragging = true;
    }
}


void PictureWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (mDragging) {
        setCursor(Qt::OpenHandCursor);
        mDragging = false;
    }
}


void PictureWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mDragging) {
        Q_ASSERT(mScrollArea != NULL);
        QPoint d = mDragStartPos - event->pos();
        mScrollArea->horizontalScrollBar()->setValue(mScrollArea->horizontalScrollBar()->value() + d.x());
        mScrollArea->verticalScrollBar()->setValue(mScrollArea->verticalScrollBar()->value() + d.y());
    }
}


void PictureWidget::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    mZoom *= pow(1.1, numSteps);
    setZoom(mZoom);
}


void PictureWidget::setZoom(qreal zoom)
{
    mZoom = zoom;
    setMinimumSize(mImage.size() * mZoom);
    resize(mImage.size() * mZoom);
    update();
}


void PictureWidget::setScrollArea(QScrollArea* scrollArea)
{
    mScrollArea = scrollArea;
}


void PictureWidget::setPicture(const QImage& img, int stripePos, bool stripeVertical)
{
    mImage = img;
    mStripePos = stripePos;
    mStripeVertical = stripeVertical;
    setMinimumSize(mImage.size() * mZoom);
    resize(mImage.size() * mZoom);
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
        qreal avgBlue,
        qreal minBrightness,
        qreal minRed,
        qreal minGreen,
        qreal minBlue)
{
    mBrightnessData = brightness;
    mRedData = red;
    mGreenData = green;
    mBlueData = blue;
    mAvgBrightness = avgBrightness;
    mAvgRed = avgRed;
    mAvgGreen = avgGreen;
    mAvgBlue = avgBlue;
    mMinBrightness = minBrightness;
    mMinRed = minRed;
    mMinGreen = minGreen;
    mMinBlue = minBlue;
    update();
}


void PictureWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.scale(mZoom, mZoom);
    painter.drawImage(QPoint(0, 0), mImage);
    painter.setBrush(Qt::NoBrush);
    if (mStripePos >= 0) {
        painter.setPen(Qt::red);
        if (mStripeVertical)
            painter.drawLine(mStripePos, 0, mStripePos, mImage.height());
        else
            painter.drawLine(0, mStripePos, mImage.width(), mStripePos);
    }
    if (mShowCurves && mBrightnessData && !mBrightnessData->isEmpty() && mMinBrightness >= 0) {
        const int y0l = mImage.height()     + mMinBrightness;
        const int y0r = mImage.height()*1/4 + mMinRed;
        const int y0g = mImage.height()*2/4 + mMinGreen;
        const int y0b = mImage.height()*3/4 + mMinBlue;
        painter.setPen(QColor(0xcc, 0xcc, 0xcc, 0xa0));
        if (mAvgBrightness >= 0)
            painter.drawLine(QLineF(0, y0l-mAvgBrightness, mImage.width(), y0l-mAvgBrightness));
        if (mAvgRed >= 0)
            painter.drawLine(QLineF(0, y0r-mAvgRed, mImage.width(), y0r-mAvgRed));
        if (mAvgGreen >= 0)
            painter.drawLine(QLineF(0, y0g-mAvgGreen, mImage.width(), y0g-mAvgGreen));
        if (mAvgBlue >= 0)
            painter.drawLine(QLineF(0, y0b-mAvgBlue, mImage.width(), y0b-mAvgBlue));

        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath lPath;
        lPath.moveTo(0, y0l);
        painter.setPen(QPen(QColor(0xff, 0xff, 0xff, 0xa0), 1.2));
        for (int i = 0; i < mBrightnessData->size(); ++i)
            lPath.lineTo(i, y0l-(*mBrightnessData)[i]);
        painter.drawPath(lPath);

        QPainterPath rPath;
        rPath.moveTo(0, y0r);
        painter.setPen(QPen(QColor(0xff, 0x00, 0x00, 0xa0), 1.2));
        for (int i = 0; i < mRedData->size(); ++i)
            rPath.lineTo(i, y0r-(*mRedData)[i]);
        painter.drawPath(rPath);

        QPainterPath gPath;
        gPath.moveTo(0, y0g);
        painter.setPen(QPen(QColor(0x00, 0xff, 0x00, 0xa0), 1.2));
        for (int i = 0; i < mGreenData->size(); ++i)
            gPath.lineTo(i, y0g-(*mGreenData)[i]);
        painter.drawPath(gPath);

        QPainterPath bPath;
        bPath.moveTo(0, y0b);
        painter.setPen(QPen(QColor(0x00, 0x00, 0xff, 0xa0), 1.2));
        for (int i = 0; i < mBlueData->size(); ++i)
            bPath.lineTo(i, y0b-(*mBlueData)[i]);
        painter.drawPath(bPath);

    }
}
