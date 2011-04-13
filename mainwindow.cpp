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
    connect(videoReaderThread, SIGNAL(percentReady(int)), this, SLOT(showPercentReady(int)));

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

    connect(ui->renderButton, SIGNAL(clicked()), this, SLOT(loadFrames()));
    connect(ui->action_Save_picture, SIGNAL(triggered()), this, SLOT(savePicture()));

    mStripeWidth = 1;
    mFrameSkip = 1;

}


MainWindow::~MainWindow()
{
    delete ui;
    delete videoWidget;
    delete pictureWidget;
}


void MainWindow::closeEvent(QCloseEvent*)
{
    pictureWidget->close();
}


void MainWindow::frameChanged(int n)
{
    QImage img;
    ui->statusBar->showMessage(tr("Seeking to %1 ms ...").arg(n), 1000);
    videoReaderThread->decoder()->seekMs(n);
    videoReaderThread->decoder()->getFrame(img);
    videoWidget->setFrame(img);
}


void MainWindow::showPercentReady(int percent)
{
    ui->statusBar->showMessage(tr("Loading %1 frames ... %2%").arg(nFrames).arg(percent));
}


void MainWindow::loadFrames(void)
{
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(nFrames));
    videoReaderThread->startReading(nFrames, &images, mFrameSkip);
}


void MainWindow::nextFrame(void)
{
    // TODO
}


void MainWindow::previousFrame(void)
{
    // TODO
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
    forward(2 * 25 * 20);
}


void MainWindow::fastBackward(void)
{
    backward(2 * 25 * 20);
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


void MainWindow::showPictureWidget(void)
{
    ui->actionPreview_picture->setChecked(true);
    setWindowState(Qt::WindowActive);
}


void MainWindow::hidePictureWidget(void)
{
    ui->actionPreview_picture->setChecked(false);
}


void MainWindow::openVideoFile(void)
{
    videoFileName = QFileDialog::getOpenFileName(this, tr("Open Video File"));
    videoReaderThread->setFile(videoFileName);
    videoWidget->setFrameSize(videoReaderThread->decoder()->frameSize());
    ui->action_CloseVideoFile->setEnabled(true);
    pictureWidget->resize(videoReaderThread->decoder()->frameSize());
    showPictureWidget();
    nFrames = videoReaderThread->decoder()->frameSize().width() * mFrameSkip / mStripeWidth;
    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(videoReaderThread->decoder()->getVideoLengthMs());
    ui->frameSlider->setSingleStep(1000);
    ui->frameSlider->setPageStep(25*1000);
    QImage img;
    videoReaderThread->decoder()->getFrame(img);
    videoWidget->setFrame(img);
    ui->renderButton->setEnabled(true);
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
    ui->action_CloseVideoFile->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
    ui->frameSlider->setValue(0);
    videoWidget->setFrame(QImage());
    pictureWidget->setPicture(QImage());
    hidePictureWidget();
    ui->renderButton->setEnabled(false);
    ui->frameSlider->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(false);
    ui->backwardButton->setEnabled(false);
    ui->fastForwardButton->setEnabled(false);
    ui->fastBackwardButton->setEnabled(false);
    ui->statusBar->showMessage(tr("File closed."));
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png *.jpg");
    pictureWidget->picture().save(saveFileName);
}


void MainWindow::decodingFinished(void)
{
    ui->action_Save_picture->setEnabled(true);
    ui->statusBar->showMessage(tr("%1 frames ready.").arg(images.size()));
    QImage dst(videoReaderThread->decoder()->frameSize(), QImage::Format_RGB888);
    for (int i = 0, x0 = 0; i < images.size(); i += mFrameSkip, x0 += mStripeWidth) {
        const QImage& src = images[i];
        for (int x = 0; x < mStripeWidth; ++x)
            for (int y = 0; y < dst.height(); ++y)
                dst.setPixel(x0+x, y, src.pixel(x0+x, y));
    }
    pictureWidget->setPicture(dst);
}
