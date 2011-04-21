/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#include <QFileDialog>
#include <QFileInfo>
#include <QtDebug>
#include <QImage>
#include <QMessageBox>
#include <QTime>
#include <QSettings>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::Company = "von-und-fuer-lau.de";
const QString MainWindow::AppName = "VStripe";


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mVideoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, mVideoWidget);
    connect(mVideoWidget, SIGNAL(fileDropped(QString)), this, SLOT(setCurrentVideoFile(QString)));

    ui->AButton->setStyleSheet("background: green");
    ui->BButton->setStyleSheet("background: red");

    connect(ui->action_OpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    connect(ui->action_CloseVideoFile, SIGNAL(triggered()), this, SLOT(closeVideoFile()));

    mFrameSlider = new MarkableSlider;
    mFrameSlider->setEnabled(false);
    ui->sliderLayout->insertWidget(0, mFrameSlider);
    connect(mFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(seekToFrame(int)));

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

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentVideoFileActs[i] = new QAction(this);
        recentVideoFileActs[i]->setVisible(false);
        connect(recentVideoFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentVideoFile()));
        recentProjectFileActs[i] = new QAction(this);
        recentProjectFileActs[i]->setVisible(false);
        connect(recentProjectFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentProjectFile()));
    }

    connect(ui->forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->backwardButton, SIGNAL(clicked()), this, SLOT(backward()));
    connect(ui->fastForwardButton, SIGNAL(clicked()), this, SLOT(fastForward()));
    connect(ui->fastBackwardButton, SIGNAL(clicked()), this, SLOT(fastBackward()));
    connect(ui->setParamsButton, SIGNAL(clicked()), this, SLOT(setParamsButtonClicked()));
    connect(ui->renderButton, SIGNAL(clicked()), this, SLOT(renderButtonClicked()));
    connect(ui->action_Save_picture, SIGNAL(triggered()), this, SLOT(savePicture()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));

    mStripeWidth = 1;
    mFrameSkip = 1;
    mFixedStripe = false;
    mEffectiveFrameNumber = -1;
    mEffectiveFrameTime = -1;
    markA = -1;
    markB = -1;
    mPreRenderFrameNumber = 0;
    connect(ui->AButton, SIGNAL(clicked()), this, SLOT(setMarkA()));
    connect(ui->BButton, SIGNAL(clicked()), this, SLOT(setMarkB()));
    connect(ui->markButton, SIGNAL(clicked()), this, SLOT(setMark()));
    connect(ui->actionClear_marks, SIGNAL(triggered()), this, SLOT(clearMarks()));
    connect(ui->prevMarkButton, SIGNAL(clicked()), this, SLOT(jumpToPrevMark()));
    connect(ui->nextMarkButton, SIGNAL(clicked()), this, SLOT(jumpToNextMark()));

    restoreAppSettings();
    ui->statusBar->showMessage(tr("Ready."), 3000);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete mVideoWidget;
    delete mPictureWidget;
    delete mFrameSlider;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    saveAppSettings();
    QMainWindow::closeEvent(event);
    mPictureWidget->close();
}


void MainWindow::saveAppSettings(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    settings.setValue("PictureWidget/geometry", mPictureWidget->saveGeometry());
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    mPictureWidget->restoreGeometry(settings.value("PictureWidget/geometry").toByteArray());
    updateRecentVideoFileActions();
    for (int i = 0; i < MaxRecentFiles; ++i)
        ui->menuOpen_recent_video->addAction(recentVideoFileActs[i]);
}


void MainWindow::setCurrentVideoFile(const QString& fileName)
{
    mVideoFileName = fileName;
    setWindowTitle(tr("%1 - %2").arg(MainWindow::AppName).arg(mVideoFileName));
    setWindowFilePath(mVideoFileName);
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(mVideoFileName);
    files.prepend(mVideoFileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentFileList", files);
    updateRecentVideoFileActions();
    if (sender() == mVideoWidget)
        loadVideoFile();
}


void MainWindow::updateRecentVideoFileActions(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentVideoFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i+1).arg(strippedName(files[i]));
        recentVideoFileActs[i]->setText(text);
        recentVideoFileActs[i]->setData(files[i]);
        recentVideoFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentVideoFileActs[j]->setVisible(false);
    if (numRecentFiles > 0)
        ui->menuOpen_recent_video->setEnabled(true);
}


QString MainWindow::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void MainWindow::openRecentVideoFile(void)
{
    QAction* action = qobject_cast<QAction *>(sender());
    if (action) {
        mVideoFileName = action->data().toString();
        loadVideoFile();
    }
}


QString MainWindow::ms2hmsz(int ms)
{
    int h, m, s, z;
    h = ms / 1000 / 60 / 60;
    m = (ms - h * 1000 * 60 * 60) / 1000 / 60;
    s = (ms - h * 1000 * 60 * 60 - m * 1000 * 60) / 1000;
    z = (ms - h * 1000 * 60 * 60 - m * 1000 * 60 - s * 1000);
    return QTime(h, m, s, z).toString("HH:mm:ss.z");
}


void MainWindow::seekToFrame(int n)
{
    QImage img;
    ui->statusBar->showMessage(tr("Seeking to frame #%1 ...").arg(n), 3000);
    mVideoReaderThread->decoder()->seekFrame(n);
    mVideoReaderThread->decoder()->getFrame(img, &mEffectiveFrameNumber, &mEffectiveFrameTime, &mDesiredFrameNumber, &mDesiredFrameTime);
    qDebug() << QString("effective: %1 (%2 ms), desired: %3 (%4 ms)").arg(mEffectiveFrameNumber).arg(mEffectiveFrameTime).arg(mDesiredFrameNumber).arg(mDesiredFrameTime);
    mVideoWidget->setFrame(img);
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


void MainWindow::startRendering(void)
{
    ui->renderButton->setText(tr("Stop rendering"));
    mFixedStripe = mVideoWidget->stripeIsFixed();
    mCurrentFrame.fill(qRgb(33, 251, 95));
    int firstFrame;
    qreal nStripes = mVideoWidget->stripeIsVertical()? (qreal)mCurrentFrame.width() : (qreal)mCurrentFrame.height();
    if (markA >= 0 && markB >= 0 && markB > markA) {
        mFrameSkip = (qreal)(markB - markA) / nStripes;
        mFrameSlider->setValue(markA);
        firstFrame = markA;
    }
    else {
        mFrameSkip = 1;
        firstFrame = mEffectiveFrameNumber;
    }
    mFrameCount = nStripes / mStripeWidth;
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(mFrameCount));
    mPreRenderFrameNumber = mFrameSlider->value();
    mVideoReaderThread->startReading(firstFrame, mFrameCount, mFrameSkip);
}


void MainWindow::stopRendering(void) {
    ui->statusBar->showMessage(tr("Rendering stopped."), 5000);
    ui->renderButton->setText(tr("Start rendering"));
    mVideoReaderThread->stopReading();
}


void MainWindow::renderButtonClicked(void)
{
    if (ui->renderButton->text() == tr("Start rendering"))
        startRendering();
    else
        stopRendering();
}


void MainWindow::forward(int nFrames)
{
    mFrameSlider->setValue(mFrameSlider->value() + nFrames * mVideoReaderThread->decoder()->getDefaultSkip());
}


void MainWindow::backward(int nFrames)
{
    mFrameSlider->setValue(mFrameSlider->value() - nFrames * mVideoReaderThread->decoder()->getDefaultSkip());
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
    qSort(mMarks);
    mFrameSlider->setB(markB);
}


void MainWindow::setMark(void)
{
    mMarks.append(mEffectiveFrameNumber);
    qSort(mMarks);
    mFrameSlider->setMarks(mMarks);
}


void MainWindow::clearMarks(void)
{
    mMarks.clear();
    mFrameSlider->setMarks(mMarks);
}


void MainWindow::jumpToNextMark(void)
{
    for (int i = 0; i < mMarks.count(); ++i)
        if (mMarks[i] > mEffectiveFrameNumber) {
            mFrameSlider->setValue(mMarks[i]);
            break;
        }
}


void MainWindow::jumpToPrevMark(void)
{
    for (int i = mMarks.count()-1; i >= 0; --i)
        if (mMarks[i] < mEffectiveFrameNumber) {
            mFrameSlider->setValue(mMarks[i]);
            break;
        }
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open video file"));
    if (fileName.isNull())
        return;
    setCurrentVideoFile(fileName);
    loadVideoFile();
}


void MainWindow::loadVideoFile(void)
{
    mVideoReaderThread->setFile(mVideoFileName);
    mVideoWidget->setFrameSize(mVideoReaderThread->decoder()->frameSize());
    ui->action_CloseVideoFile->setEnabled(true);
    mCurrentFrame = QImage(mVideoReaderThread->decoder()->frameSize(), QImage::Format_RGB888);
    mPictureWidget->resize(mVideoReaderThread->decoder()->frameSize());
    mPictureWidget->setPicture(QImage());
    showPictureWidget();
    mFrameSlider->setValue(0);
    QImage img;
    int lastFrameNumber;
    ui->infoPlainTextEdit->appendPlainText(QString("%1 (%2)").arg(mVideoReaderThread->decoder()->formatCtx()->iformat->long_name).arg(mVideoReaderThread->decoder()->codec()->long_name));
    ui->statusBar->showMessage(tr("Finding last frame ..."));
    mVideoReaderThread->decoder()->seekMs(mVideoReaderThread->decoder()->getVideoLengthMs());
    mVideoReaderThread->decoder()->getFrame(img, &lastFrameNumber);
    qDebug() << "Last frame is # " << lastFrameNumber;
    qDebug() << "Video length is " << mVideoReaderThread->decoder()->getVideoLengthMs() << " ms";
    mFrameSlider->setMaximum(lastFrameNumber);
    mFrameSlider->setValue(0);
    ui->actionSave_project->setEnabled(true);
    ui->actionSave_project_as->setEnabled(true);
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
    ui->statusBar->showMessage(tr("File closed."), 5000);
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png, *.jpg");
    mPictureWidget->picture().save(saveFileName);
}


void MainWindow::frameReady(QImage src, int frameNumber)
{
    // qDebug() << QString("frameReady(..., %1)").arg(frameNumber, 3, 10, QChar('0'));
    int srcpos = mFixedStripe? mVideoWidget->stripePos() : frameNumber * mStripeWidth;
    int dstpos = frameNumber * mStripeWidth;
    if (mVideoWidget->stripeIsVertical()) {
        for (int x = 0; x < mStripeWidth; ++x)
            for (int y = 0; y < src.height(); ++y)
                mCurrentFrame.setPixel(dstpos + x, y, src.pixel(srcpos + x, y));
    }
    else {
        for (int y = 0; y < mStripeWidth; ++y)
            for (int x = 0; x < src.width(); ++x)
                mCurrentFrame.setPixel(x, dstpos + y, src.pixel(x, srcpos + y));
    }
    mPictureWidget->setPicture(mCurrentFrame);
    mFrameSlider->blockSignals(true);
    mFrameSlider->setValue(mPreRenderFrameNumber + (int)(frameNumber * mFrameSkip));
    mFrameSlider->blockSignals(false);
}


void MainWindow::decodingFinished()
{
    ui->action_Save_picture->setEnabled(true);
    ui->renderButton->setText(tr("Start rendering"));
    mPreRenderFrameNumber = mFrameSlider->value();
}


void MainWindow::enableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(true);
    ui->setParamsButton->setEnabled(true);
    ui->renderButton->setEnabled(true);
    mFrameSlider->setEnabled(true);
    ui->AButton->setEnabled(true);
    ui->BButton->setEnabled(true);
    ui->prevMarkButton->setEnabled(true);
    ui->nextMarkButton->setEnabled(true);
    ui->markButton->setEnabled(true);
    ui->forwardButton->setEnabled(true);
    ui->backwardButton->setEnabled(true);
    ui->fastForwardButton->setEnabled(true);
    ui->fastBackwardButton->setEnabled(true);
    ui->action_CloseVideoFile->setEnabled(true);
    ui->action_Save_picture->setEnabled(true);
    ui->actionClear_marks->setEnabled(true);
}

void MainWindow::disableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(false);
    ui->setParamsButton->setEnabled(false);
    ui->renderButton->setEnabled(false);
    mFrameSlider->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->BButton->setEnabled(false);
    ui->prevMarkButton->setEnabled(false);
    ui->nextMarkButton->setEnabled(false);
    ui->markButton->setEnabled(false);
    ui->forwardButton->setEnabled(false);
    ui->backwardButton->setEnabled(false);
    ui->fastForwardButton->setEnabled(false);
    ui->fastBackwardButton->setEnabled(false);
    ui->action_CloseVideoFile->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
    ui->actionClear_marks->setEnabled(false);
}


void MainWindow::about(void)
{
    QMessageBox::about(this, tr("About %1").arg(MainWindow::AppName),
        tr("<p><strong>%1</strong> &ndash; Generate synchroballistic photography alike images from footage.</p>"
           "<p>Copyright (c) 2011 Oliver Lau &lt;oliver@ersatzworld.net&gt;</p>"
           "<p>VideoDecoder Copyright (c) 2009-2010 by Daniel Roggen &lt;droggen@gmail.com&gt;</p>"
           "<p>All rights reserved.</p>").arg(MainWindow::AppName));
}


void MainWindow::help(void)
{
    QMessageBox::information(this, tr("Help on %1").arg(MainWindow::AppName),
        tr("<p>Not implemented yet</p>"));
}


void MainWindow::openProject(void)
{
    QMessageBox::information(this, QString(), tr("<p>Not implemented yet</p>"));
}


void MainWindow::saveProject(void)
{
    if (mProjectFileName.isEmpty()) {
        saveProjectAs();
        return;
    }
    QMessageBox::information(this, QString(), tr("<p>Not implemented yet</p>"));
}


void MainWindow::saveProjectAs(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save project file as ..."));
    if (fileName.isNull())
        return;
    mProjectFileName = fileName;

}


void MainWindow::updateRecentProjectFileActions(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentProjectFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i+1).arg(strippedName(files[i]));
        recentProjectFileActs[i]->setText(text);
        recentProjectFileActs[i]->setData(files[i]);
        recentProjectFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentProjectFileActs[j]->setVisible(false);
}


void MainWindow::openRecentProjectFile(void)
{

}
