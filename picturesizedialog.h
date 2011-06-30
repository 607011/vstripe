#ifndef PICTURESIZEDIALOG_H
#define PICTURESIZEDIALOG_H

#include <QDialog>

namespace Ui {
    class PictureSizeDialog;
}

class PictureSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PictureSizeDialog(const QSize& currentSize, bool stripeIsVertical, QWidget* parent = 0);
    ~PictureSizeDialog();
    QSize requestedSize(void) const;

private:
    Ui::PictureSizeDialog *ui;
    QSize mOldSize;
    bool mStripeIsVertical;
};


#endif // PICTURESIZEDIALOG_H
