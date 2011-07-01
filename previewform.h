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
    void setSizeConstraints(const QSize& minimumSize, const QSize& maximumSize, const QSize& defaultSize);
    PictureWidget* pictureWidget(void) { return mPictureWidget; }
    QSlider* brightnessSlider(void);
    QSlider* redSlider(void);
    QSlider* greenSlider(void);
    QSlider* blueSlider(void);
    QDial* factorDial(void);
    qreal amplificationCorrection(void) const;

public slots:
    void resetRGBLCorrections(void);
    void amplificationChanged(void);
    void choosePictureSize(void);

signals:
    void correctionsChanged(void);
    void visibilityChanged(bool);
    void pictureSizeChanged(QSize);

protected:
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

private:
    Ui::PreviewForm *ui;
    PictureWidget* mPictureWidget;
    bool mStripeIsVertical;
    QSize mDefaultSize;
    QSize mMaximumSize;
    bool mDragging;
    QPoint mDragStartPos;
    QPoint mPreviousPos;
    qreal mZoom;
    static const QString WinTitle;

};

#endif // __PREVIEWFORM_H_
