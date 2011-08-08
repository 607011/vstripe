/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PICTUREWIDGET_H_
#define __PICTUREWIDGET_H_

#include <QtGui/QWidget>
#include <QImage>
#include <QResizeEvent>
#include <QScrollArea>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimerEvent>
#include <QtCore/QVector>

#include "histogram.h"


class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    PictureWidget(QWidget* parent = NULL);
    QSize sizeHint(void) const { return QSize(1920, 1080); }
    QSize minimumSizeHint(void) const { return QSize(320, 240); }
    void setPicture(const QImage& img, int stripePos);
    void setStripeOrientation(bool stripeVertical);
    void setBrightnessData(
            BrightnessData* brightness,
            BrightnessData* red,
            BrightnessData* green,
            BrightnessData* blue,
            qreal avgBrightness = -1,
            qreal avgRed = -1,
            qreal avgGreen = -1,
            qreal avgBlue = -1,
            qreal minBrightness = -1,
            qreal minRed = -1,
            qreal minGreen = -1,
            qreal minBlue = -1);
    inline const QImage& picture(void) const { return mImage; }
    void setZoom(qreal);

public slots:
    void copyImageToClipboard(void);
    void showCurves(bool);
    void mirror(bool);

protected:
    void paintEvent(QPaintEvent*);
    void wheelEvent(QWheelEvent*);

private: // methods

private: // variables
    QSize mScrollAreaSize;
    bool mMirrored;
    QImage mImage;
    bool mShowCurves;
    BrightnessData* mBrightnessData;
    BrightnessData* mRedData;
    BrightnessData* mGreenData;
    BrightnessData* mBlueData;
    qreal mAvgBrightness;
    qreal mAvgRed;
    qreal mAvgGreen;
    qreal mAvgBlue;
    qreal mMinBrightness;
    qreal mMinRed;
    qreal mMinGreen;
    qreal mMinBlue;
    int mStripePos;
    bool mStripeVertical;
    int mMouseSteps;
    qreal mZoom;
};

#endif // __PICTUREWIDGET_H_
