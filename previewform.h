/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __PREVIEWFORM_H_
#define __PREVIEWFORM_H_

#include <QWidget>
#include <QSlider>
#include <QDial>
#include "picturewidget.h"


namespace Ui {
    class PreviewForm;
}

class PreviewForm : public QWidget
{
    Q_OBJECT

public:
    PreviewForm(QWidget* parent = NULL);
    ~PreviewForm();
    void setSizeConstraint(const QSize& minimum, const QSize& maximum);
    PictureWidget* pictureWidget(void) { return mPictureWidget; }
    QSlider* brightnessSlider(void);
    QSlider* redSlider(void);
    QSlider* greenSlider(void);
    QSlider* blueSlider(void);
    QDial* factorDial(void);

public slots:
    void resetRGBLCorrections(void);

signals:
    void correctionsChanged(void);
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
