/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PICTURESIZEDIALOG_H_
#define __PICTURESIZEDIALOG_H_

#include <QDialog>

namespace Ui {
    class PictureSizeDialog;
}

class PictureSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PictureSizeDialog(const QSize& currentSize, const QSize& defaultSize, const QSize& maximumSize, bool stripeIsVertical, QWidget* parent = NULL);
    ~PictureSizeDialog();
    QSize requestedSize(void) const;

private slots:
    void resetSize(void);
    void optimizeSize(void);

private:
    Ui::PictureSizeDialog *ui;
    QSize mOldSize;
    QSize mDefaultSize;
    QSize mMaximumSize;
    bool mStripeIsVertical;
};


#endif // __PICTURESIZEDIALOG_H_
