/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QMessageBox>
#include <qmath.h>

#include "previewform.h"
#include "ui_previewform.h"
#include "picturesizedialog.h"


const QString PreviewForm::WinTitle = QObject::tr("VStripe - Picture Preview");

QSlider* PreviewForm::brightnessSlider(void) { return ui->exposureLSlider; }
QSlider* PreviewForm::redSlider(void) { return ui->exposureRSlider; }
QSlider* PreviewForm::greenSlider(void) { return ui->exposureGSlider; }
QSlider* PreviewForm::blueSlider(void) { return ui->exposureBSlider; }
QDial* PreviewForm::factorDial(void) { return ui->dialCorrectionFactor; }


PreviewForm::PreviewForm(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::PreviewForm),
        mStripeIsVertical(true)
{
    ui->setupUi(this);

    mPictureWidget = new PictureWidget;
    mPictureWidget->showCurves(ui->checkBoxShowCurves->isChecked());

    ui->scrollArea->setWidget(mPictureWidget);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    QObject::connect(ui->resetRGBLButton, SIGNAL(clicked()), this, SLOT(resetRGBLCorrections()));
    QObject::connect(ui->checkBoxShowCurves, SIGNAL(toggled(bool)), mPictureWidget, SLOT(showCurves(bool)));
    QObject::connect(ui->pushButtonPictureSize, SIGNAL(clicked()), this, SLOT(choosePictureSize()));
    QObject::connect(ui->dialCorrectionFactor, SIGNAL(valueChanged(int)), this, SLOT(amplificationChanged()));

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


void PreviewForm::choosePictureSize(void)
{
    PictureSizeDialog diag(mPictureWidget->picture().size(), mDefaultSize, mMaximumSize, mStripeIsVertical);
    int rc = diag.exec();
    if (rc == QDialog::Accepted)
        emit pictureSizeChanged(diag.requestedSize());
}


void PreviewForm::setSizeConstraints(const QSize& minimumSize, const QSize& maximumSize, const QSize& defaultSize)
{
    mMaximumSize = maximumSize;
    mDefaultSize = defaultSize;
    mPictureWidget->setMinimumSize(minimumSize);
    mPictureWidget->setMaximumSize(maximumSize);
    mStripeIsVertical = (minimumSize.width() == 0);
}


void PreviewForm::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier)) {
        mPictureWidget->copyImageToClipboard();
    }
    else if (event->key() == Qt::Key_Escape) {
        mPictureWidget->setZoom(1.0);
        mPictureWidget->move(0, 0);
    }
}


void PreviewForm::closeEvent(QCloseEvent*)
{
    emit visibilityChanged(false);
}
