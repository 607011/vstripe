/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "previewform.h"
#include "ui_previewform.h"

PreviewForm::PreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::PreviewForm)
{
    ui->setupUi(this);

    mPictureWidget = new PictureWidget;
    ui->horizontalLayout->insertWidget(0, mPictureWidget);
}


QSlider* PreviewForm::levelSlider(void)
{
    return ui->exposureLevelSlider;
}


PreviewForm::~PreviewForm()
{
    delete ui;
}
