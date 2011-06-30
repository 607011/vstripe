#include "picturesizedialog.h"
#include "ui_picturesizedialog.h"

PictureSizeDialog::PictureSizeDialog(const QSize& currentSize, bool stripeIsVertical, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PictureSizeDialog),
    mOldSize(currentSize),
    mStripeIsVertical(stripeIsVertical)
{
    ui->setupUi(this);
    if (stripeIsVertical) {
        ui->spinBoxWidth->setEnabled(true);
        ui->spinBoxHeight->setEnabled(false);
    }
    else {
        ui->spinBoxWidth->setEnabled(false);
        ui->spinBoxHeight->setEnabled(true);
    }
    ui->spinBoxWidth->setValue(currentSize.width());
    ui->spinBoxHeight->setValue(currentSize.height());
}


PictureSizeDialog::~PictureSizeDialog()
{
    delete ui;
}


QSize PictureSizeDialog::requestedSize(void) const
{
    return mStripeIsVertical ?
            QSize(ui->spinBoxWidth->value(), mOldSize.height()) :
            QSize(mOldSize.width(), ui->spinBoxHeight->value());
}
