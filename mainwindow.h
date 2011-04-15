/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QMainWindow>

#include "videoreaderthread.h"
#include "videowidget.h"
#include "picturewidget.h"
#include "markableslider.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = NULL);
    ~MainWindow();

    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    QSize sizeHint(void) const { return QSize(1280, 720); }

public slots:
    void openVideoFile(void);
    void closeVideoFile(void);
    void decodingFinished(void);
    void togglePictureWidget(bool);
    void frameChanged(int);
    void forward(int nFrames = 1);
    void backward(int nFrames = 1);
    void fastForward(void);
    void fastBackward(void);
    void setMarkA(void);
    void setMarkB(void);
    void loadFrames(void);
    void savePicture(void);
    void showPercentReady(int);
    void frameReady(QImage, int);

    void pictureWidthSet(int);


protected:
    void closeEvent(QCloseEvent*);

private: // variables
    Ui::MainWindow* ui;
    QString mVideoFileName;
    MarkableSlider* mFrameSlider;
    VideoWidget* mVideoWidget;
    PictureWidget* mPictureWidget;
    VideoReaderThread* mVideoReaderThread;
    int markA;
    int markB;
    int nFrames;
    int mStripeWidth; // Streifen dieser Breite (Pixel) werden von jedem eingelesenen Frame behalten
    int mFrameSkip; // so viel Frames werden pro Frame beim Einlesen übersprungen
    bool mFixedStripe; // true: der Streifen bleibt fest an der gewählten Position; false: der Streifen bewegt sich mit jedem Frame um mStripeWidth Pixel weiter
    QImage mFrame;
    int mEffectiveFrameNumber;
    int mEffectiveFrameTime;

private: // methods
    void showPictureWidget(void);
    void hidePictureWidget(void);
};

#endif // MAINWINDOW_H
