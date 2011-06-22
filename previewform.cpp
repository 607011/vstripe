/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QMessageBox>
#include <qmath.h>

#include "previewform.h"
#include "ui_previewform.h"

const QString PreviewForm::WinTitle = QObject::tr("VStripe - Picture Preview");

QSlider* PreviewForm::brightnessSlider(void) { return ui->exposureLSlider; }
QSlider* PreviewForm::redSlider(void) { return ui->exposureRSlider; }
QSlider* PreviewForm::greenSlider(void) { return ui->exposureGSlider; }
QSlider* PreviewForm::blueSlider(void) { return ui->exposureBSlider; }
QDial* PreviewForm::factorDial(void) { return ui->dialCorrectionFactor; }


PreviewForm::PreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::PreviewForm)
{
    ui->setupUi(this);

    mPictureWidget = new PictureWidget;
    ui->horizontalLayout->insertWidget(0, mPictureWidget);

    connect(ui->resetRGBLButton, SIGNAL(clicked()), this, SLOT(resetRGBLCorrections()));
    connect(ui->checkBoxShowCurves, SIGNAL(toggled(bool)), mPictureWidget, SLOT(showCurves(bool)));

    connect(ui->dialCorrectionFactor, SIGNAL(valueChanged(int)), this, SLOT(amplificationChanged()));

    mPictureWidget->showCurves(ui->checkBoxShowCurves->isChecked());

    ui->dialCorrectionFactor->setValue(10);
    amplificationChanged();
}


PreviewForm::~PreviewForm()
{
    delete ui;
    delete mPictureWidget;
}


void PreviewForm::resetRGBLCorrections(void)
{
    if (QMessageBox::warning(this, tr("Reset RGB/L corrections?"), tr("Really reset RGB/L corrections?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        ui->exposureLSlider->setValue(0);
        ui->exposureRSlider->setValue(0);
        ui->exposureGSlider->setValue(0);
        ui->exposureBSlider->setValue(0);
        ui->dialCorrectionFactor->setValue(10);
        emit correctionsChanged();
    }
}


void PreviewForm::amplificationChanged(void)
{
    ui->lineEditAmplification->setText(QString("x%L1").arg(11*amplificationCorrection(), 0, 'g', 3));
}


qreal PreviewForm::amplificationCorrection(void) const
{
    return (qreal)(ui->dialCorrectionFactor->value() - ui->dialCorrectionFactor->minimum())
            / (qreal)(ui->dialCorrectionFactor->maximum() - ui->dialCorrectionFactor->minimum());
}



void PreviewForm::setSizeConstraint(const QSize& minimum, const QSize& maximum)
{
    mPictureWidget->setMinimumSize(minimum);
    mPictureWidget->setMaximumSize(maximum);
}


void PreviewForm::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
        mPictureWidget->copyImageToClipboard();
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
