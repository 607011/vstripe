/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QtGlobal>

#include "videowidget.h"


VideoWidget::VideoWidget(QWidget* parent) : QWidget(parent)
{
    setAcceptDrops(true);
    setBaseSize(480, 270);
    setMinimumSize(384, 216);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mStripePos = -1;
    mStripeWidth = 1;
    mDragging = false;
    mVerticalStripe = true;
    mDrawHistogram = true;
    mCalculating = false;
    for (int i = 0; i < HistogramBinCount; ++i)
        mHistogram[i] = 0;
}


int VideoWidget::stripePos(void) const
{
    if (mVerticalStripe)
        return (mDestRect.width() != 0)?  mStripePos * mImage.width()  / mDestRect.width()  : -1;
    else
        return (mDestRect.height() != 0)? mStripePos * mImage.height() / mDestRect.height() : -1;
}


void VideoWidget::calcDestRect(void) {
    if (mWindowAspectRatio < mFrameAspectRatio) {
        const int h = int(width() / mFrameAspectRatio);
        mDestRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = int(height() * mFrameAspectRatio);
        mDestRect = QRect((width()-w)/2, 0, w, height());
    }
}


void VideoWidget::setFrameSize(const QSize& sz) {
    mFrameAspectRatio = qreal(sz.width()) / qreal(sz.height());
    calcDestRect();
    if (!mImage.isNull())
        update();
}


void VideoWidget::setStripeWidth(int stripeWidth)
{
    mStripeWidth = stripeWidth;
    if (!mImage.isNull())
        update();
}


void VideoWidget::setStripePos(int pos)
{
    mStripePos = pos;
    if (!mImage.isNull())
        update();
}


void VideoWidget::setStripeOrientation(bool vertical)
{
    mVerticalStripe = vertical;
    if (!mImage.isNull())
        update();
}


/**
  @returns overall brightness
  */
qreal VideoWidget::calcHistogram(void)
{
    const QImage& img = mImage;
    const qreal N = 1.0 / (img.width() * img.height());
    qreal L = 0;
    for (int i = 0; i < HistogramBinCount; ++i)
        mHistogram[i] = 0;
    for (int y = 0; y < img.height(); ++y) {
        const QRgb* d = reinterpret_cast<const QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x, ++d) {
            const int v0 = (qRed(*d) + qGreen(*d) + qBlue(*d)) / 3;
            const qreal v = v0 * N;
            mHistogram[v0] += v;
            L += v;
        }
    }
    mHistogramMax = 0;
    for (int i = 0; i < HistogramBinCount; ++i)
        if (mHistogram[i] > mHistogramMax)
            mHistogramMax = mHistogram[i];
    return L;
}



void VideoWidget::setFrame(QImage img)
{
    Q_ASSERT(mCalculating == false);
    mCalculating = true;
    mImage = img;
    calcHistogram();
    update();
    mCalculating = false;
}


void VideoWidget::resizeEvent(QResizeEvent* e)
{
    mWindowAspectRatio = qreal(e->size().width()) / qreal(e->size().height());
    calcDestRect();
    update();
}


void VideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // draw background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(0, 0, width(), height());
    if (!mImage.isNull())
        painter.drawImage(mDestRect, mImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    if (mDrawHistogram) {
        const int hh = 192, x0 = 10, y0 = 10;
        const qreal hs = (int) (hh / mHistogramMax);
        painter.setPen(QColor(0xff, 0xff, 0xff, 0x99));
        painter.setBrush(QColor(0xff, 0xff, 0xff, 0x66));
        painter.drawRect(x0, y0, 256, 192);
        painter.setPen(QColor(0x33, 0x33, 0x33, 0x80));
        painter.setBrush(Qt::NoBrush);
        for (int i = 0; i < HistogramBinCount; ++i)
            painter.drawLine(x0+i, y0+hh, x0+i, y0+hh-(int)(mHistogram[i]*hs));
    }
    // draw stripe or direction marker
    if (mVerticalStripe) {
        if (mStripePos >= 0) {
            painter.setBrush(QColor(0xff, 0x00, 0x00, 0xcc));
            painter.drawRect(mStripePos + mDestRect.x(), mDestRect.y(), mStripeWidth, mDestRect.height());
        }
        else {
            painter.setPen(QColor(0x00, 0x00, 0xff, 0x99));
            painter.setBrush(QColor(0x00, 0x00, 0xff, 0x66));
            QPainterPath path;
            path.moveTo(width()/2+width()/20, height()/2);
            path.lineTo(width()/2-width()/20, height()/2-height()/10);
            path.lineTo(width()/2-width()/20, height()/2+height()/10);
            path.lineTo(width()/2+width()/20, height()/2);
            painter.drawPath(path);
        }
    }
    else {
        if (mStripePos >= 0) {
            painter.setBrush(QColor(0xff, 0x00, 0x00, 0xcc));
            painter.drawRect(mDestRect.x(), mStripePos + mDestRect.y(), mDestRect.width(), mStripeWidth);
        }
        else {
            painter.setPen(QColor(0x00, 0x00, 0xff, 0x99));
            painter.setBrush(QColor(0x00, 0x00, 0xff, 0x66));
            QPainterPath path;
            path.moveTo(width()/2,            height()/2+height()/20);
            path.lineTo(width()/2-width()/20, height()/2-height()/20);
            path.lineTo(width()/2+width()/20, height()/2-height()/20);
            path.lineTo(width()/2,            height()/2+height()/20);
            painter.drawPath(path);
        }
    }
}


void VideoWidget::calcStripePos(void)
{
    if (mVerticalStripe) {
        int sPos = mMousePos.x() - mDestRect.x();
        if (sPos != mStripePos) {
            mStripePos = sPos;
            if (mStripePos >= mDestRect.width())
                mStripePos = -1;
            emit stripePosChanged(stripePos());
        }
    }
    else {
        int sPos = mMousePos.y() - mDestRect.y();
        if (sPos != mStripePos) {
            mStripePos = sPos;
            if (mStripePos >= mDestRect.height())
                mStripePos = -1;
            emit stripePosChanged(stripePos());
        }
    }
}


void VideoWidget::keyPressEvent(QKeyEvent* event)
{
    if (mDragging) {
        mVerticalStripe = ((event->modifiers() & Qt::ControlModifier) == 0);
        calcStripePos();
        emit stripeOrientationChanged(mVerticalStripe);
        update();
    }
}


void VideoWidget::mouseMoveEvent(QMouseEvent* event)
{
    mMousePos = event->pos();
    if (mDragging) {
        bool vStripe = ((event->modifiers() & Qt::ControlModifier) == 0);
        if (vStripe != mVerticalStripe) {
            mVerticalStripe = vStripe;
            emit stripeOrientationChanged(mVerticalStripe);
        }
        calcStripePos();
        update();
    }
}


void VideoWidget::mousePressEvent(QMouseEvent* event)
{
    mDragging = (event->button() == Qt::LeftButton);
    if (mDragging)
        setFocus(Qt::MouseFocusReason);
}


void VideoWidget::dragEnterEvent(QDragEnterEvent* event)
{
    // only accept local files
    if (event->mimeData()->hasUrls()) {
        bool accepted = true;
        foreach (const QUrl& url, event->mimeData()->urls()) {
            accepted &= !url.toLocalFile().isEmpty();
            if (!accepted)
                break;
        }
        if (accepted)
            event->acceptProposedAction();
    }
}


void VideoWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl>& urls = event->mimeData()->urls();
        if (!urls.isEmpty())
            emit fileDropped(urls.first().toLocalFile());
    }
}
