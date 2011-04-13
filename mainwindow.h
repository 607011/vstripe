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
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    QSize minimumSizeHint(void) const { return QSize(720, 576); }

public slots:
    void openVideoFile(void);
    void closeVideoFile(void);
    void decodingFinished(void);
    void togglePictureWidget(bool);
    void toggleReading(void);
    void frameChanged(int);
    void forward(int nFrames = 1);
    void backward(int nFrames = 1);
    void fastForward(void);
    void fastBackward(void);
    void setMarkA(void);
    void setMarkB(void);

private:
    Ui::MainWindow* ui;
    QString videoFileName;
    VideoWidget* videoWidget;
    PictureWidget* pictureWidget;
    VideoReaderThread* videoReaderThread;
    QVector<QImage> images;
    int markA;
    int markB;

};

#endif // MAINWINDOW_H
