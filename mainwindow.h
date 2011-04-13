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

public slots:
    void openVideoFile(void);
    void closeVideoFile(void);
    void decodingFinished(void);
    void togglePictureWidget(bool);
    void frameChanged(int);
    void nextFrame(void);
    void previousFrame(void);
    void forward(int nFrames = 20);
    void backward(int nFrames = 20);
    void fastForward(void);
    void fastBackward(void);
    void setMarkA(void);
    void setMarkB(void);
    void loadFrames(void);
    void savePicture(void);
    void showPercentReady(int);

protected:
    void closeEvent(QCloseEvent*);

private:
    Ui::MainWindow* ui;
    QString videoFileName;
    VideoWidget* videoWidget;
    PictureWidget* pictureWidget;
    VideoReaderThread* videoReaderThread;
    QVector<QImage> images;
    int markA;
    int markB;
    int nFrames;
    int mStripeWidth;
    int mFrameSkip;

    void showPictureWidget(void);
    void hidePictureWidget(void);
};

#endif // MAINWINDOW_H
