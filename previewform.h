/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef PREVIEWFORM_H
#define PREVIEWFORM_H

#include <QWidget>
#include <QSlider>
#include "picturewidget.h"

namespace Ui {
    class PreviewForm;
}

class PreviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewForm(QWidget* parent = 0);
    ~PreviewForm();

    PictureWidget* pictureWidget(void) { return mPictureWidget; }
    QSlider* levelSlider(void);

private:
    Ui::PreviewForm *ui;

    PictureWidget* mPictureWidget;
};

#endif // PREVIEWFORM_H
