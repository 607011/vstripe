/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PREVIEWFORM_H_
#define __PREVIEWFORM_H_

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
    explicit PreviewForm(QWidget* parent = NULL);
    ~PreviewForm();
    void setVisible(bool visible);
    void setSizeConstraint(const QSize&, const QSize&);
    PictureWidget* pictureWidget(void) { return mPictureWidget; }
    QSlider* brightnessSlider(void);
    QSlider* redSlider(void);
    QSlider* greenSlider(void);
    QSlider* blueSlider(void);

signals:
    void visibilityChanged(bool);
    void sizeChanged(const QSize&);

protected:
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Ui::PreviewForm *ui;
    PictureWidget* mPictureWidget;
    static const QString WinTitle;

};

#endif // __PREVIEWFORM_H_
