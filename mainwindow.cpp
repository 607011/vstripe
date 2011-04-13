/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QFileDialog>
#include <QtDebug>
#include <QImage>
#include <QMessageBox>

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
    connect(ui->action_CloseVideoFile, SIGNAL(triggered()), this, SLOT(closeVideoFile()));

    connect(ui->startStopButton, SIGNAL(clicked()), this, SLOT(toggleReading()));
    ui->startStopButton->setText(tr("Start"));

    ui->statusBar->showMessage(tr("Ready."), 3000);

    videoReaderThread = new VideoReaderThread(videoWidget);
    connect(videoReaderThread, SIGNAL(finished()), this, SLOT(decodingFinished()));

    pictureWidget = new PictureWidget;

    connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        pictureWidget->show();
    connect(pictureWidget, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));

    connect(ui->frameSlider, SIGNAL(valueChanged(int)), this, SLOT(frameChanged(int)));

    connect(ui->forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->backwardButton, SIGNAL(clicked()), this, SLOT(backward()));
    connect(ui->fastForwardButton, SIGNAL(clicked()), this, SLOT(fastForward()));
    connect(ui->fastBackwardButton, SIGNAL(clicked()), this, SLOT(fastBackward()));

    markA = -1;
    markB = -1;
    connect(ui->AButton, SIGNAL(clicked()), this, SLOT(setMarkA()));
    connect(ui->BButton, SIGNAL(clicked()), this, SLOT(setMarkB()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete videoWidget;
    delete pictureWidget;
    qDebug() << "~MainWindow()";
}


void MainWindow::frameChanged(int n)
{
    videoWidget->setFrame(images[n]);
}


void MainWindow::forward(int nFrames)
{
    ui->frameSlider->setValue(ui->frameSlider->value() + nFrames);
}


void MainWindow::backward(int nFrames)
{
    ui->frameSlider->setValue(ui->frameSlider->value() - nFrames);
}


void MainWindow::fastForward(void)
{
    forward(25);
}


void MainWindow::fastBackward(void)
{
    backward(25);
}


void MainWindow::setMarkA(void)
{
    markA = ui->frameSlider->value();
}


void MainWindow::setMarkB(void)
{
    markB = ui->frameSlider->value();
}


void MainWindow::togglePictureWidget(bool visible)
{
    pictureWidget->setVisible(visible);
    if (visible)
        pictureWidget->showNormal();
    setWindowState(Qt::WindowActive);
}


void MainWindow::toggleReading(void)
{
    if (ui->startStopButton->text() == tr("Start")) {
        qDebug() << "Starting decode ...";
        ui->startStopButton->setText(tr("Stop"));
        images.clear();
        videoReaderThread->startReading(100, &images);
    }
    else {
        qDebug() << "Stopping decode ...";
        ui->startStopButton->setText(tr("Start"));
        decodingFinished();
    }
}


void MainWindow::openVideoFile(void)
{
    videoFileName = QFileDialog::getOpenFileName(this, tr("Open Video File"));
    videoReaderThread->setFile(videoFileName);
    ui->startStopButton->setEnabled(true);
    toggleReading();
}


void MainWindow::closeVideoFile(void)
{
    images.clear();
    ui->frameSlider->setValue(0);
    ui->frameSlider->setEnabled(false);
    videoWidget->setFrame(QImage());
}


void MainWindow::decodingFinished(void)
{
    ui->startStopButton->setEnabled(false);
    ui->startStopButton->setText(tr("Start"));
    videoReaderThread->stopReading();
    ui->frameSlider->setEnabled(true);
    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(images.size() - 1);
    ui->frameSlider->setValue(0);
    frameChanged(0);
    QMessageBox::information(this, tr("Ready."), tr("%1 frames decoded.").arg(images.size()));
}
