/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QFileDialog>
#include <QtDebug>
#include <QImage>
#include <QMessageBox>
#include <QTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mVideoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, mVideoWidget);

    ui->AButton->setStyleSheet("background: green");
    ui->BButton->setStyleSheet("background: red");

    connect(ui->action_OpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    connect(ui->action_CloseVideoFile, SIGNAL(triggered()), this, SLOT(closeVideoFile()));

    mFrameSlider = new MarkableSlider;
    mFrameSlider->setEnabled(false);
    ui->sliderLayout->insertWidget(0, mFrameSlider);
    connect(mFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(frameChanged(int)));

    mVideoReaderThread = new VideoReaderThread;
    connect(mVideoReaderThread, SIGNAL(finished()), this, SLOT(decodingFinished()));
    connect(mVideoReaderThread, SIGNAL(percentReady(int)), this, SLOT(showPercentReady(int)));
    connect(mVideoReaderThread, SIGNAL(frameReady(QImage,int)), this, SLOT(frameReady(QImage,int)));
    connect(mVideoReaderThread, SIGNAL(frameReady(QImage,int)), mVideoWidget, SLOT(setFrame(QImage)));

    mPictureWidget = new PictureWidget;
    connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        mPictureWidget->show();
    connect(mPictureWidget, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));

    connect(ui->forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->backwardButton, SIGNAL(clicked()), this, SLOT(backward()));
    connect(ui->fastForwardButton, SIGNAL(clicked()), this, SLOT(fastForward()));
    connect(ui->fastBackwardButton, SIGNAL(clicked()), this, SLOT(fastBackward()));


    connect(ui->renderButton, SIGNAL(clicked()), this, SLOT(loadFrames()));
    connect(ui->action_Save_picture, SIGNAL(triggered()), this, SLOT(savePicture()));

    mStripeWidth = 1;
    mFrameSkip = 1;
    mFixedStripe = false;
    mEffectiveFrameNumber = -1;
    mEffectiveFrameTime = -1;
    markA = -1;
    markB = -1;
    connect(ui->AButton, SIGNAL(clicked()), this, SLOT(setMarkA()));
    connect(ui->BButton, SIGNAL(clicked()), this, SLOT(setMarkB()));

    ui->statusBar->showMessage(tr("Ready."), 3000);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete mVideoWidget;
    delete mPictureWidget;
    delete mFrameSlider;
}


void MainWindow::closeEvent(QCloseEvent*)
{
    mPictureWidget->close();
}


static QString ms2hmsz(int ms)
{
    int h, m, s, z;
    h = ms / 1000 / 60 / 60;
    m = (ms - h * 1000 * 60 * 60) / 1000 / 60;
    s = (ms - h * 1000 * 60 * 60 - m * 1000 * 60) / 1000;
    z = (ms - h * 1000 * 60 * 60 - m * 1000 * 60 - s * 1000);
    return QTime(h, m, s, z).toString("HH:mm:ss.z");
}


void MainWindow::frameChanged(int n)
{
    QImage img;
    ui->statusBar->showMessage(tr("Seeking to frame #%1 ...").arg(n));
    mVideoReaderThread->decoder()->seekFrame(n);
    mVideoReaderThread->decoder()->getFrame(img, &mEffectiveFrameNumber, &mEffectiveFrameTime);
    mVideoWidget->setFrame(img);
    ui->statusBar->showMessage(QString());
    ui->frameNumberLineEdit->setText(tr("%1").arg(mEffectiveFrameNumber));
    ui->frameTimeLineEdit->setText(ms2hmsz(mEffectiveFrameTime));
}


void MainWindow::showPercentReady(int percent)
{
    ui->statusBar->showMessage(tr("Loading %1 frames ... %2%").arg(nFrames).arg(percent), 1000);
}


void MainWindow::pictureWidthSet(int w)
{

}


void MainWindow::loadFrames(void)
{
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(nFrames));
    ui->renderButton->setEnabled(false);
    mFrame.fill(qRgb(33, 251, 95));
    mVideoReaderThread->startReading(nFrames, mFrameSkip);
}


void MainWindow::forward(int nFrames)
{
    mFrameSlider->setValue(mFrameSlider->value() + nFrames);
}


void MainWindow::backward(int nFrames)
{
    mFrameSlider->setValue(mFrameSlider->value() - nFrames);
}


void MainWindow::fastForward(void)
{
    forward(50);
}


void MainWindow::fastBackward(void)
{
    backward(50);
}


void MainWindow::setMarkA(void)
{
    markA = ui->AButton->isChecked()? mEffectiveFrameNumber : -1;
    mFrameSlider->setA(markA);
}


void MainWindow::setMarkB(void)
{
    markB = ui->AButton->isChecked()? mEffectiveFrameNumber : -1;
    mFrameSlider->setB(markB);
}


void MainWindow::togglePictureWidget(bool visible)
{
    mPictureWidget->setVisible(visible);
    if (visible)
        mPictureWidget->showNormal();
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
    mVideoFileName = QFileDialog::getOpenFileName(this, tr("Open Video File"));
    if (mVideoFileName.isNull())
        return;
    mVideoReaderThread->setFile(mVideoFileName);
    mVideoWidget->setFrameSize(mVideoReaderThread->decoder()->frameSize());
    ui->action_CloseVideoFile->setEnabled(true);
    mFrame = QImage(mVideoReaderThread->decoder()->frameSize(), QImage::Format_RGB888);
    mPictureWidget->resize(mVideoReaderThread->decoder()->frameSize());
    mPictureWidget->setPicture(QImage());
    showPictureWidget();
    nFrames = mVideoReaderThread->decoder()->frameSize().width() * mFrameSkip / mStripeWidth;
    mFrameSlider->setValue(0);
    QImage img;
    int lastFrameNumber;
    ui->statusBar->showMessage(tr("Finding last frame ..."));
    mVideoReaderThread->decoder()->seekMs(mVideoReaderThread->decoder()->getVideoLengthMs());
    mVideoReaderThread->decoder()->getFrame(img, &lastFrameNumber);
    qDebug() << "Last frame is # " << lastFrameNumber;
    qDebug() << "Video length is " << mVideoReaderThread->decoder()->getVideoLengthMs() << " ms";
    mVideoReaderThread->decoder()->seekFrame(0);
    frameChanged(0);
    mFrameSlider->setMaximum(lastFrameNumber);
    ui->renderButton->setEnabled(true);
    mFrameSlider->setEnabled(true);
    ui->AButton->setEnabled(true);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(true);
    ui->backwardButton->setEnabled(true);
    ui->fastForwardButton->setEnabled(true);
    ui->fastBackwardButton->setEnabled(true);
}


void MainWindow::closeVideoFile(void)
{
    ui->action_CloseVideoFile->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
    mFrameSlider->setValue(0);
    mVideoWidget->setFrame(QImage());
    mPictureWidget->setPicture(QImage());
    hidePictureWidget();
    ui->renderButton->setEnabled(false);
    mFrameSlider->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(false);
    ui->backwardButton->setEnabled(false);
    ui->fastForwardButton->setEnabled(false);
    ui->fastBackwardButton->setEnabled(false);
    ui->frameNumberLineEdit->setText(QString());
    ui->frameTimeLineEdit->setText(QString());
    ui->statusBar->showMessage(tr("File closed."));
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png *.jpg");
    mPictureWidget->picture().save(saveFileName);
}


void MainWindow::frameReady(QImage src, int frameNumber)
{
    int i = frameNumber * mFrameSkip;
    int srcx = (mFixedStripe)? (mFrame.width() - mStripeWidth) / 2 : i * mStripeWidth;
    int dstx = frameNumber * mStripeWidth;
    for (int x = 0; x < mStripeWidth; ++x)
        for (int y = 0; y < src.height(); ++y)
            mFrame.setPixel(dstx + x, y, src.pixel(srcx + x, y));
    qDebug() << "received frame number " << frameNumber;
    mPictureWidget->setPicture(mFrame);
}


void MainWindow::decodingFinished()
{
    ui->action_Save_picture->setEnabled(true);
    ui->renderButton->setEnabled(true);
}
