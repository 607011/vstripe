/*
 * Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * $Id$
 */

#include <QFileDialog>
#include <QtDebug>
#include <QImage>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    videoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, videoWidget);

    ui->AButton->setStyleSheet("background: green");
    ui->BButton->setStyleSheet("background: red");

    connect(ui->action_OpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    connect(ui->startStopButton, SIGNAL(clicked()), this, SLOT(closeVideoFile()));

    ui->statusBar->showMessage(tr("Registering Codecs ..."));
    av_register_all();

    ui->statusBar->showMessage(tr("Ready."), 3000);

    videoReaderThread = new VideoReaderThread(videoWidget);

    pictureWidget = new PictureWidget;

    connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        pictureWidget->show();
    connect(pictureWidget, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete videoWidget;
    delete pictureWidget;
    qDebug() << "~MainWindow()";
}


void MainWindow::togglePictureWidget(bool visible)
{
    pictureWidget->setVisible(visible);
    if (visible)
        pictureWidget->showNormal();
    setWindowState(Qt::WindowActive);
}


void MainWindow::openVideoFile(void)
{
    videoFileName = QFileDialog::getOpenFileName(this, tr("Open Video File"));
    connect(videoReaderThread, SIGNAL(finished()), this, SLOT(closeVideoFile()));
    ui->startStopButton->setEnabled(true);
    videoReaderThread->startReading(videoFileName);
}


void MainWindow::closeVideoFile(void)
{
    videoReaderThread->stopReading();
    ui->startStopButton->setEnabled(false);
}
