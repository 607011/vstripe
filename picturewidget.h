/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PICTUREWIDGET_H_
#define __PICTUREWIDGET_H_

#include <QWidget>
#include <QImage>
#include <QResizeEvent>

#include "histogram.h"

class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    PictureWidget(QWidget* parent = NULL);
    QSize sizeHint(void) const { return QSize(1920, 1080); }
    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    void setPicture(const QImage&);
    void setBrightnessData(
            const BrightnessData* brightness,
            const BrightnessData* red,
            const BrightnessData* green,
            const BrightnessData* blue,
            qreal avgBrightness = -1,
            qreal avgRed = -1,
            qreal avgGreen = -1,
            qreal avgBlue = -1,
            qreal minBrightness = -1,
            qreal minRed = -1,
            qreal minGreen = -1,
            qreal minBlue = -1);
    inline const QImage& picture(void) const { return mImage; }

public slots:
    void copyImageToClipboard(void);

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    QImage mImage;
    const BrightnessData* mBrightnessData;
    const BrightnessData* mRedData;
    const BrightnessData* mGreenData;
    const BrightnessData* mBlueData;
    qreal mAvgBrightness;
    qreal mAvgRed;
    qreal mAvgGreen;
    qreal mAvgBlue;
    qreal mMinBrightness;
    qreal mMinRed;
    qreal mMinGreen;
    qreal mMinBlue;
};

#endif // __PICTUREWIDGET_H_
