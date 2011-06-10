/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include "previewform.h"
#include "ui_previewform.h"

const QString PreviewForm::WinTitle = QObject::tr("VStripe - Picture Preview");


PreviewForm::PreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::PreviewForm)
{
    ui->setupUi(this);

    mPictureWidget = new PictureWidget;
    ui->horizontalLayout->insertWidget(0, mPictureWidget);
}


PreviewForm::~PreviewForm()
{
    delete ui;
    delete mPictureWidget;
}


QSlider* PreviewForm::brightnessSlider(void) { return ui->exposureLSlider; }
QSlider* PreviewForm::redSlider(void) { return ui->exposureRSlider; }
QSlider* PreviewForm::greenSlider(void) { return ui->exposureGSlider; }
QSlider* PreviewForm::blueSlider(void) { return ui->exposureBSlider; }


void PreviewForm::setVisible(bool visible)
{
    QWidget::setVisible(visible);
}


void PreviewForm::setSizeConstraint(const QSize& minimum, const QSize& maximum)
{
    mPictureWidget->setMinimumSize(minimum);
    mPictureWidget->setMaximumSize(maximum);
}


void PreviewForm::closeEvent(QCloseEvent*)
{
    emit visibilityChanged(false);
}


void PreviewForm::resizeEvent(QResizeEvent*)
{
    emit sizeChanged(mPictureWidget->size());
    setWindowTitle(QString("%1 - %2x%3").arg(WinTitle).arg(mPictureWidget->width()).arg(mPictureWidget->height()));
}
