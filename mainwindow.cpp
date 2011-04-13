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
    QImage img;
    qDebug() << "seeking to " << n << " ms";
    videoReaderThread->decoder()->seekMs(n);
    videoReaderThread->decoder()->getFrame(img);
    videoWidget->setFrame(img);
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
    forward(25 * 20);
}


void MainWindow::fastBackward(void)
{
    backward(25 * 20);
}


void MainWindow::setMarkA(void)
{
    if (ui->AButton->isChecked()) {
        markA = -1;
        ui->AButton->setChecked(false);
    }
    else {
        markA = ui->frameSlider->value();
        ui->AButton->setChecked(true);
    }
}


void MainWindow::setMarkB(void)
{
    if (ui->BButton->isChecked()) {
        markB = -1;
        ui->BButton->setChecked(false);
    }
    else {
        markB = ui->frameSlider->value();
        ui->BButton->setChecked(true);
    }
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
    videoReaderThread->setFile(videoFileName);
    videoWidget->setFrameSize(videoReaderThread->decoder()->frameSize());
    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(videoReaderThread->decoder()->getVideoLengthMs());
    ui->frameSlider->setSingleStep(1000);
    ui->frameSlider->setPageStep(25*1000);
    QImage img;
    videoReaderThread->decoder()->getFrame(img);
    videoWidget->setFrame(img);
    ui->frameSlider->setEnabled(true);
    ui->AButton->setEnabled(true);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(true);
    ui->backwardButton->setEnabled(true);
    ui->fastForwardButton->setEnabled(true);
    ui->fastBackwardButton->setEnabled(true);
}


void MainWindow::closeVideoFile(void)
{
    images.clear();
    ui->frameSlider->setValue(0);
    videoWidget->setFrame(QImage());
    ui->frameSlider->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(false);
    ui->backwardButton->setEnabled(false);
    ui->fastForwardButton->setEnabled(false);
    ui->fastBackwardButton->setEnabled(false);
}


void MainWindow::decodingFinished(void)
{
    videoReaderThread->stopReading();
}
