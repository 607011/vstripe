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
    connect(ui->setParamsButton, SIGNAL(clicked()), this, SLOT(setParamsButtonClicked()));
    connect(ui->renderButton, SIGNAL(clicked()), this, SLOT(renderButtonClicked()));
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
    ui->statusBar->showMessage(tr("Loading %1 frames ... %2%").arg(mFrameCount).arg(percent), 1000);
}


void MainWindow::pictureWidthSet(int)
{
    // TODO
    qDebug() << "MainWindow::pictureWidthSet() is not implemented yet.";
}


void MainWindow::render(void)
{
    ui->renderButton->setText(tr("Stop rendering"));
    mFixedStripe = mVideoWidget->stripeFixed();
    mFrame.fill(qRgb(33, 251, 95));
    int firstFrame;
    if (markA >= 0 && markB >= 0 && markB > markA) {
        mFrameSkip = (qreal)(markB - markA) / (mVideoWidget->stripeIsVertical())? (qreal)mFrame.width() : (qreal)mFrame.height();
        mFrameSlider->setValue(markA);
        firstFrame = markA;
    }
    else {
        mFrameSkip = 1;
        firstFrame = mEffectiveFrameNumber;
    }
    mFrameCount = ((mVideoWidget->stripeIsVertical())? mVideoReaderThread->decoder()->frameSize().width() : mVideoReaderThread->decoder()->frameSize().height()) * mFrameSkip / mStripeWidth;
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(mFrameCount));
    mVideoReaderThread->startReading(firstFrame, mFrameCount, mVideoWidget->stripeIsVertical(), mFrameSkip);
}


void MainWindow::stopRendering(void) {
    ui->statusBar->showMessage(tr("Rendering stopped."), 5000);
    ui->renderButton->setText(tr("Start rendering"));
    mVideoReaderThread->stopReading();
}


void MainWindow::renderButtonClicked(void)
{
    if (ui->renderButton->text() == tr("Start rendering"))
        render();
    else
        stopRendering();
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


void MainWindow::setParamsButtonClicked(void)
{
    mFrameSlider->setValue(ui->frameNumberLineEdit->text().toInt());
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
    enableGuiButtons();
}


void MainWindow::closeVideoFile(void)
{
    disableGuiButtons();
    ui->frameNumberLineEdit->setText(QString());
    ui->frameTimeLineEdit->setText(QString());
    mFrameSlider->setValue(0);
    mVideoWidget->setFrame(QImage());
    mPictureWidget->setPicture(QImage());
    hidePictureWidget();
    ui->statusBar->showMessage(tr("File closed."));
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png, *.jpg");
    mPictureWidget->picture().save(saveFileName);
}


void MainWindow::frameReady(QImage src, int frameNumber)
{
    int srcpos = mFixedStripe? mVideoWidget->stripePos() : frameNumber * mStripeWidth;
    int dstpos = frameNumber * mStripeWidth;
    if (mVideoWidget->stripeIsVertical()) {
        if (frameNumber >= mFrame.width()) {
            qDebug() << "frameNumber >= mFrame.width(): " << frameNumber;
            return;
        }
        for (int x = 0; x < mStripeWidth; ++x)
            for (int y = 0; y < src.height(); ++y)
                mFrame.setPixel(dstpos + x, y, src.pixel(srcpos + x, y));
    }
    else {
        if (frameNumber >= mFrame.height()) {
            qDebug() << "frameNumber >= mFrame.height(): " << frameNumber;
            return;
        }
        for (int y = 0; y < mStripeWidth; ++y)
            for (int x = 0; x < src.width(); ++x)
                mFrame.setPixel(x, dstpos + y, src.pixel(x, srcpos + y));
    }
    mPictureWidget->setPicture(mFrame);
}


void MainWindow::decodingFinished()
{
    ui->action_Save_picture->setEnabled(true);
    ui->renderButton->setText(tr("Start rendering"));
}


void MainWindow::enableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(true);
    ui->setParamsButton->setEnabled(true);
    ui->renderButton->setEnabled(true);
    mFrameSlider->setEnabled(true);
    ui->AButton->setEnabled(true);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(true);
    ui->backwardButton->setEnabled(true);
    ui->fastForwardButton->setEnabled(true);
    ui->fastBackwardButton->setEnabled(true);
    ui->action_CloseVideoFile->setEnabled(true);
    ui->action_Save_picture->setEnabled(true);
}

void MainWindow::disableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(false);
    ui->setParamsButton->setEnabled(false);
    ui->renderButton->setEnabled(false);
    mFrameSlider->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->BButton->setEnabled(true);
    ui->forwardButton->setEnabled(false);
    ui->backwardButton->setEnabled(false);
    ui->fastForwardButton->setEnabled(false);
    ui->fastBackwardButton->setEnabled(false);
    ui->action_CloseVideoFile->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
}
