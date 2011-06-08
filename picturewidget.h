/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>
#include <QImage>
#include <QResizeEvent>

#include "histogram.h"

class PictureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget* parent = NULL);
    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    void setPicture(const QImage&);
    void setBrightnessData(const BrightnessData*, qreal avgBrightness = -1);
    inline const QImage& picture(void) const { return mImage; }

protected:
    void paintEvent(QPaintEvent*);

private:
    QImage mImage;
    const BrightnessData* mBrightnessData;
    qreal mAvgBrightness;

};

#endif // PICTUREWIDGET_H
