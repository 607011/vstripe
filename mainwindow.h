#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QMainWindow>

#include "videoreaderthread.h"
#include "videowidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    void openVideoFile(void);
    void closeVideoFile(void);

private:
    Ui::MainWindow* ui;
    QString videoFileName;
    VideoWidget* videoWidget;

    VideoReaderThread* videoReaderThread;
};

#endif // MAINWINDOW_H
