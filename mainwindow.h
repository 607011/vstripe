/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
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
    void togglePictureWidget(bool);

private:
    Ui::MainWindow* ui;
    QString videoFileName;
    VideoWidget* videoWidget;
    PictureWidget* pictureWidget;
    VideoReaderThread* videoReaderThread;

};

#endif // MAINWINDOW_H
