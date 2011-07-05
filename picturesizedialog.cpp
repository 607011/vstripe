/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <QtCore/QtDebug>

#include "picturesizedialog.h"
#include "ui_picturesizedialog.h"

PictureSizeDialog::PictureSizeDialog(const QSize& currentSize, const QSize& defaultSize, const QSize& maximumSize, bool stripeIsVertical, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PictureSizeDialog),
    mOldSize(currentSize),
    mDefaultSize(defaultSize),
    mMaximumSize(maximumSize),
    mStripeIsVertical(stripeIsVertical)
{
    ui->setupUi(this);
    if (stripeIsVertical)
        ui->spinBoxWidth->setEnabled(true);
    else
        ui->spinBoxHeight->setEnabled(true);
    ui->spinBoxWidth->setMinimum(1);
    ui->spinBoxWidth->setMaximum(maximumSize.width());
    ui->spinBoxWidth->setValue(currentSize.isNull()? defaultSize.width() : currentSize.width());
    ui->spinBoxHeight->setMinimum(1);
    ui->spinBoxHeight->setMaximum(maximumSize.height());
    ui->spinBoxHeight->setValue(currentSize.isNull()? defaultSize.height() : currentSize.height());
    QObject::connect(ui->pushButtonReset, SIGNAL(clicked()), this, SLOT(resetSize()));
    QObject::connect(ui->pushButtonOptimize, SIGNAL(clicked()), this, SLOT(optimizeSize()));
}


PictureSizeDialog::~PictureSizeDialog()
{
    delete ui;
}


void PictureSizeDialog::resetSize(void)
{
    ui->spinBoxWidth->setValue(mDefaultSize.width());
    ui->spinBoxHeight->setValue(mDefaultSize.height());
}


void PictureSizeDialog::optimizeSize(void)
{
    if (mStripeIsVertical)
        ui->spinBoxWidth->setValue(mMaximumSize.width());
    else
        ui->spinBoxHeight->setValue(mMaximumSize.height());
}


QSize PictureSizeDialog::requestedSize(void) const
{
    return mStripeIsVertical ?
            QSize(ui->spinBoxWidth->value(), mOldSize.height()) :
            QSize(mOldSize.width(), ui->spinBoxHeight->value());
}
