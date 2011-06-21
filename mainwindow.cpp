/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
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
#include <QFileInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "histogram.h"

const QString MainWindow::Company = "von-und-fuer-lau.de";
const QString MainWindow::AppName = "VStripe";
#ifndef QT_NO_DEBUG
const QString MainWindow::AppVersion = "0.9.4 DEBUG";
#else
const QString MainWindow::AppVersion = "0.9.4";
#endif


MainWindow::MainWindow(int argc, char* argv[], QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("%1 %2").arg(MainWindow::AppName).arg(MainWindow::AppVersion));

    qRegisterMetaType<Histogram>();

    mVideoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, mVideoWidget);
    connect(mVideoWidget, SIGNAL(fileDropped(QString)), this, SLOT(fileDropped(QString)));
    connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), this, SLOT(setStripeOrientation(bool)));
    connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), &mProject, SLOT(setStripeOrientation(bool)));
    connect(mVideoWidget, SIGNAL(stripePosChanged(int)), &mProject, SLOT(setStripePos(int)));

    connect(ui->action_OpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    connect(ui->action_CloseVideoFile, SIGNAL(triggered()), this, SLOT(closeVideoFile()));
    connect(ui->actionAutofitPreview, SIGNAL(triggered()), this, SLOT(autoFitPreview()));
    connect(ui->actionHistogram, SIGNAL(toggled(bool)), mVideoWidget, SLOT(setHistogramEnabled(bool)));
    connect(ui->actionClear_histogram_region, SIGNAL(triggered()), this, SLOT(clearHistogramRegion()));

    mFrameSlider = new MarkableSlider(&mProject);
    mFrameSlider->setEnabled(false);
    ui->sliderLayout->insertWidget(0, mFrameSlider);
    connect(mFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(seekToFrame(int)));

    mVideoReaderThread = new VideoReaderThread;
    connect(mVideoReaderThread, SIGNAL(finished()), this, SLOT(decodingFinished()));
    connect(mVideoReaderThread, SIGNAL(percentReady(int)), this, SLOT(showPercentReady(int)));
    connect(mVideoReaderThread, SIGNAL(frameReady(QImage,Histogram,int,int,int)), this, SLOT(frameReady(QImage,Histogram,int,int,int)));
    // connect(mVideoReaderThread, SIGNAL(frameReady(QImage,Histogram,int,int,int)), mVideoWidget, SLOT(setFrame(QImage,Histogram)));
    connect(mVideoWidget, SIGNAL(histogramRegionChanged(QRect)), mVideoReaderThread, SLOT(setHistogramRegion(QRect)));
    connect(mVideoWidget, SIGNAL(histogramRegionChanged(QRect)), &mProject, SLOT(setHistogramRegion(QRect)));

    mPreviewForm = new PreviewForm;
    connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        mPreviewForm->show();
    connect(mPreviewForm, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));
    connect(mPreviewForm, SIGNAL(sizeChanged(const QSize&)), this, SLOT(setPictureSize(const QSize&)));
    connect(mPreviewForm->brightnessSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    connect(mPreviewForm->redSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    connect(mPreviewForm->greenSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    connect(mPreviewForm->blueSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    connect(ui->actionReset_RGB_L_levels, SIGNAL(triggered()), this, SLOT(resetRGBL()));

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
    connect(ui->actionOpen_project, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->actionSave_project, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui->actionSave_project_as, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
    connect(ui->actionCopy_to_clipboard, SIGNAL(triggered()), mPreviewForm->pictureWidget(), SLOT(copyImageToClipboard()));

    mEffectiveFrameNumber = Project::INVALID_FRAME;
    mEffectiveFrameTime = -1;
    mPreRenderFrameNumber = 0;
    mProject.setCurrentFrame(0);
    connect(ui->AButton, SIGNAL(clicked()), this, SLOT(setMarkA()));
    connect(ui->BButton, SIGNAL(clicked()), this, SLOT(setMarkB()));
    connect(ui->markButton, SIGNAL(clicked()), this, SLOT(setMark()));
    connect(ui->actionClear_marks, SIGNAL(triggered()), this, SLOT(clearMarks()));
    connect(ui->prevMarkButton, SIGNAL(clicked()), this, SLOT(jumpToPrevMark()));
    connect(ui->nextMarkButton, SIGNAL(clicked()), this, SLOT(jumpToNextMark()));

    restoreAppSettings();
    ui->statusBar->showMessage(tr("Ready."), 3000);

    if (argc > 1)
        mFileNameFromCmdLine = argv[1];
}


MainWindow::~MainWindow()
{
    delete ui;
    delete mVideoWidget;
    delete mPreviewForm;
    delete mFrameSlider;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    saveAppSettings();
    QMainWindow::closeEvent(event);
    mPreviewForm->close();
}


void MainWindow::saveAppSettings(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    settings.setValue("PreviewForm/geometry", mPreviewForm->saveGeometry());
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    mPreviewForm->restoreGeometry(settings.value("PreviewForm/geometry").toByteArray());
    updateRecentVideoFileActions();
    updateRecentProjectFileActions();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        ui->menuOpen_recent_video->addAction(recentVideoFileActs[i]);
        ui->menuOpen_recent_project->addAction(recentProjectFileActs[i]);
    }
}


void MainWindow::setCurrentVideoFile(const QString& fileName)
{
    mProject.setVideoFileName(fileName);
    setWindowTitle(tr("%1 %2 - %3").arg(MainWindow::AppName).arg(MainWindow::AppVersion).arg(mProject.videoFileName()));
    setWindowFilePath(mProject.videoFileName());
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentVideoFileList").toStringList();
    files.removeAll(mProject.videoFileName());
    files.prepend(mProject.videoFileName());
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentVideoFileList", files);
    updateRecentVideoFileActions();
}


void MainWindow::setCurrentProjectFile(const QString& fileName)
{
    mProject.setFileName(fileName);
    setWindowFilePath(mProject.fileName());
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentProjectFileList").toStringList();
    files.removeAll(mProject.fileName());
    files.prepend(mProject.fileName());
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentProjectFileList", files);
    updateRecentProjectFileActions();
}


void MainWindow::fileDropped(const QString& fileName)
{
    Q_ASSERT(!fileName.isNull());

    QFileInfo fi(fileName);
    if (fi.isFile() && fi.isReadable()) {
        if (fileName.endsWith(".xml") || fileName.endsWith(".vstripe"))
            openProject(fileName);
        else {
            setCurrentVideoFile(fileName);
            loadVideoFile();
            clearMarks();
        }
    }
    else QMessageBox::critical(this, tr("File does not exist"), tr("File '%1' does not exist").arg(fileName));
}


void MainWindow::updateRecentVideoFileActions(void)
{
    QSettings settings(MainWindow::Company, MainWindow::AppName);
    QStringList files = settings.value("recentVideoFileList").toStringList();
    int numRecentFiles = qMin(files.size(), int(MaxRecentFiles));
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
        QString fileName(action->data().toString());
        QFileInfo fi(fileName);
        if (fi.isFile() && fi.isReadable()) {
            setCurrentVideoFile(fileName);
            loadVideoFile();
            clearMarks();
        }
        else
            QMessageBox::critical(this, tr("File does not exist"), tr("File '%1' does not exist").arg(fileName));
    }
}


QString MainWindow::ms2hmsz(int ms, bool withMs)
{
    int h, m, s, z;
    h = ms / 1000 / 60 / 60;
    m = (ms - h * 1000 * 60 * 60) / 1000 / 60;
    s = (ms - h * 1000 * 60 * 60 - m * 1000 * 60) / 1000;
    z = (ms - h * 1000 * 60 * 60 - m * 1000 * 60 - s * 1000);
    return withMs? QTime(h, m, s, z).toString("HH:mm:ss.z") : QTime(h, m, s).toString("HH:mm:ss");
}


void MainWindow::seekToFrame(int n)
{
    setCursor(Qt::WaitCursor);
    mPreviewForm->setCursor(Qt::WaitCursor);
    mProject.setCurrentFrame(n);
    QImage img;
    ui->statusBar->showMessage(tr("Seeking to frame #%1 ...").arg(n), 3000);
    mVideoReaderThread->decoder()->seekFrame(n);
    mVideoReaderThread->decoder()->getFrame(img, &mEffectiveFrameNumber, &mEffectiveFrameTime, &mDesiredFrameNumber, &mDesiredFrameTime);
    // qDebug() << QString("effective: %1 (%2 ms), desired: %3 (%4 ms)").arg(mEffectiveFrameNumber).arg(mEffectiveFrameTime).arg(mDesiredFrameNumber).arg(mDesiredFrameTime);
    if (ui->actionHistogram->isChecked())
        mVideoReaderThread->calcHistogram(img);
    mVideoWidget->setFrame(img, mVideoReaderThread->histogram(), -1);
    ui->frameNumberLineEdit->setText(QString("%1").arg(mEffectiveFrameNumber));
    ui->frameTimeLineEdit->setText(ms2hmsz(mEffectiveFrameTime));
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
}


void MainWindow::showPercentReady(int percent)
{
    ui->statusBar->showMessage(tr("Loading %1 frames ... %2%").arg(mFrameCount).arg(percent), 1000);
}


void MainWindow::setStripeOrientation(bool vertical)
{
    if (mCurrentFrame.isNull())
        return;
    if (vertical)
        mPreviewForm->setSizeConstraint(QSize(0, mVideoReaderThread->decoder()->frameSize().height()), QSize(QWIDGETSIZE_MAX, mVideoReaderThread->decoder()->frameSize().height()));
    else
        mPreviewForm->setSizeConstraint(QSize(mVideoReaderThread->decoder()->frameSize().width(), 0), QSize(mVideoReaderThread->decoder()->frameSize().width(), QWIDGETSIZE_MAX));
    mPreviewForm->resize(mVideoReaderThread->decoder()->frameSize());
}


void MainWindow::setPictureSize(const QSize& size)
{
    if (mVideoReaderThread->isRunning())
        stopRendering();
    mCurrentFrame = QImage(size, QImage::Format_RGB888);
}


void MainWindow::startRendering(void)
{
    ui->renderButton->setText(tr("Stop rendering"));
    disablePreviewForm();
    mFrameBrightness.clear();
    mFrameRed.clear();
    mFrameGreen.clear();
    mFrameBlue.clear();
    mMinTotalBrightness = INT_MAX;
    mMinTotalRed = INT_MAX;
    mMinTotalGreen = INT_MAX;
    mMinTotalBlue = INT_MAX;
    mPreviewForm->pictureWidget()->setBrightnessData(&mFrameBrightness, &mFrameRed, &mFrameGreen, &mFrameBlue);
    int firstFrame, lastFrame;
    mFrameCount = mVideoWidget->stripeIsVertical()? qreal(mCurrentFrame.width()) : qreal(mCurrentFrame.height());
    if (mProject.markA() != Project::INVALID_FRAME && mProject.markB() != Project::INVALID_FRAME && mProject.markB() > mProject.markA()) {
        mFrameSlider->setValue(mProject.markA());
        firstFrame = mProject.markA();
        lastFrame = mProject.markB();
    }
    else {
        firstFrame = mEffectiveFrameNumber;
        lastFrame = mLastFrameNumber;
    }
    mFrameDelta = qreal(lastFrame - firstFrame) / mFrameCount;
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(mFrameCount));
    mPreRenderFrameNumber = mFrameSlider->value();
    mProject.setCurrentFrame(mPreRenderFrameNumber);
    mVideoReaderThread->startReading(firstFrame, mFrameCount, mFrameDelta);
}


void MainWindow::stopRendering(void) {
    ui->statusBar->showMessage(tr("Rendering stopped."), 5000);
    ui->renderButton->setText(tr("Start rendering"));
    mVideoReaderThread->stopReading();
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
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
    mProject.setMarkA(ui->AButton->isChecked()? mEffectiveFrameNumber : Project::INVALID_FRAME);
    mFrameSlider->update();
    if (mProject.markBIsSet())
        ui->infoPlainTextEdit->appendPlainText(tr("%1 frames selected").arg(mProject.markB() - mProject.markA()));
}


void MainWindow::setMarkB(void)
{
    mProject.setMarkB(ui->BButton->isChecked()? mEffectiveFrameNumber : Project::INVALID_FRAME);
    mFrameSlider->update();
    if (mProject.markAIsSet())
        ui->infoPlainTextEdit->appendPlainText(tr("%1 frames selected").arg(mProject.markB() - mProject.markA()));
}


void MainWindow::setMark(void)
{
    mProject.appendMark(Project::mark_type(mEffectiveFrameNumber, QString()));
    mFrameSlider->update();
}


void MainWindow::clearMarks(void)
{
    mProject.clearMarks();
    updateButtons();
    mFrameSlider->update();
}


void MainWindow::clearHistogramRegion(void)
{
    mProject.setHistogramRegion(QRect());
    mVideoReaderThread->setHistogramRegion(QRect());
    mVideoWidget->setHistogramRegion(QRect());
}


void MainWindow::jumpToPrevMark(void)
{
    for (int i = mProject.marks().count()-1; i >= 0; --i) {
        const int frame = mProject.marks()[i].frame;
        if (frame < mEffectiveFrameNumber) {
            mFrameSlider->setValue(frame);
            break;
        }
    }
}


void MainWindow::jumpToNextMark(void)
{
    for (int i = 0; i < mProject.marks().count(); ++i) {
        const int frame = mProject.marks()[i].frame;
        if (frame > mEffectiveFrameNumber) {
            mFrameSlider->setValue(frame);
            break;
        }
    }
}


void MainWindow::setParamsButtonClicked(void)
{
    mFrameSlider->setValue(ui->frameNumberLineEdit->text().toInt());
}


void MainWindow::togglePictureWidget(bool visible)
{
    mPreviewForm->setVisible(visible);
    if (visible)
        mPreviewForm->showNormal();
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


void MainWindow::frameReady(const QImage& src, const Histogram& histogram, int frameNumber, int effectiveFrameNumber, int effectiveFrameTime)
{
    Q_ASSERT(!mCurrentFrame.isNull());
    Q_ASSERT(!src.isNull());
    mFrameBrightness.append(histogram.totalBrightness());
    mFrameRed.append(histogram.totalRed());
    mFrameGreen.append(histogram.totalGreen());
    mFrameBlue.append(histogram.totalBlue());
    if (histogram.totalBrightness() < mMinTotalBrightness)
        mMinTotalBrightness = histogram.totalBrightness();
    if (histogram.totalRed() < mMinTotalRed)
        mMinTotalRed = histogram.totalRed();
    if (histogram.totalGreen() < mMinTotalGreen)
        mMinTotalGreen = histogram.totalGreen();
    if (histogram.totalBlue() < mMinTotalBlue)
        mMinTotalBlue = histogram.totalBlue();
    const int dstpos = frameNumber;
    const int srcpos = mProject.stripeIsFixed() ?
                       mProject.stripePos() :
                       (frameNumber % (mProject.stripeIsVertical()? src.width() : src.height()));
    if (mProject.stripeIsVertical()) {
        for (int y = 0; y < src.height(); ++y)
            mCurrentFrame.setPixel(dstpos, y, src.pixel(srcpos, y));
    }
    else {
        for (int x = 0; x < src.width(); ++x)
            mCurrentFrame.setPixel(x, dstpos, src.pixel(x, srcpos));
    }
    mPreviewForm->pictureWidget()->setPicture(mCurrentFrame, srcpos);
    mVideoWidget->setFrame(src, histogram, srcpos);
    mFrameSlider->blockSignals(true);
    mFrameSlider->setValue(mPreRenderFrameNumber + int(frameNumber * mFrameDelta));
    mFrameSlider->blockSignals(false);
    ui->frameNumberLineEdit->setText(QString("%1").arg(effectiveFrameNumber));
    ui->frameTimeLineEdit->setText(ms2hmsz(effectiveFrameTime));
}


qreal MainWindow::averageBrightnessData(const BrightnessData& v) {
    qreal sum = 0;
    for (BrightnessData::const_iterator i = v.begin(); i != v.end(); ++i)
        sum += *i;
    return sum / v.count();
}


void MainWindow::decodingFinished()
{
    ui->action_Save_picture->setEnabled(true);
    ui->renderButton->setText(tr("Start rendering"));
    mPreRenderFrameNumber = mFrameSlider->value();
    mProject.setCurrentFrame(mPreRenderFrameNumber);
    mAvgBrightness = averageBrightnessData(mFrameBrightness);
    mAvgRed = averageBrightnessData(mFrameRed);
    mAvgGreen = averageBrightnessData(mFrameGreen);
    mAvgBlue = averageBrightnessData(mFrameBlue);
    mPreviewForm->pictureWidget()->setBrightnessData(
            &mFrameBrightness, &mFrameRed, &mFrameGreen, &mFrameBlue,
            mAvgBrightness, mAvgRed, mAvgGreen, mAvgBlue,
            mMinTotalBrightness, mMinTotalRed, mMinTotalGreen, mMinTotalBlue);

    // guess an appropriate brightness correction factor
    qreal diffSum = 0;
    for (BrightnessData::const_iterator i = mFrameBrightness.begin(); i != mFrameBrightness.end(); ++i)
        if (*i > mAvgBrightness)
            diffSum += qAbs(*i - mAvgBrightness);
    if (mFrameBrightness.count() > 0)
        diffSum /= mFrameBrightness.count();
    ui->infoPlainTextEdit->appendPlainText(tr("avg. luminance error: %1").arg(diffSum));
    mPreviewForm->brightnessSlider()->setValue((int)(diffSum*6.7));

    mVideoWidget->setRunningStripePos(-1);

    deflicker();
    enablePreviewForm();
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
}


int MainWindow::darker(int v, int factor)
{
    if (factor <= 0)
        return v;
    else if (factor < 100)
        return lighter(v, 10000 / factor);
    v = (v * 100) / factor;
    return v;
}


int MainWindow::lighter(int v, int factor)
{
    if (factor <= 0)
        return v;
    else if (factor < 100)
        return darker(v, 10000 / factor);
    v = (factor * v) / 100;
    if (v > UCHAR_MAX)
        v = UCHAR_MAX;
    return v;
}


void MainWindow::deflicker(void)
{
    setCursor(Qt::BusyCursor);
    mPreviewForm->setCursor(Qt::BusyCursor);
    qreal lLevel = (qreal)mPreviewForm->brightnessSlider()->value()/100;
    qreal rLevel = (qreal)mPreviewForm->redSlider()->value()/100;
    qreal gLevel = (qreal)mPreviewForm->greenSlider()->value()/100;
    qreal bLevel = (qreal)mPreviewForm->blueSlider()->value()/100;
    mProject.setBrightnessLevel(lLevel);
    mProject.setRedLevel(rLevel);
    mProject.setGreenLevel(gLevel);
    mProject.setBlueLevel(bLevel);
    if (lLevel > 0 || rLevel != 0 || gLevel != 0 || bLevel != 0) {
        QImage img(mCurrentFrame.size(), mCurrentFrame.format());
        if (mVideoWidget->stripeIsVertical()) {
            if (img.width() != mFrameBrightness.count())
                return;
            for (int x = 0; x < mFrameBrightness.count(); ++x) {
                int dl = (int) ((mFrameBrightness[x] - mAvgBrightness) * lLevel);
                int dr = (int) ((mFrameRed[x] - mAvgRed) * rLevel);
                int dg = (int) ((mFrameGreen[x] - mAvgGreen) * gLevel);
                int db = (int) ((mFrameBlue[x] - mAvgBlue) * bLevel);
                for (int y = 0; y < img.height(); ++y) {
                    QRgb rgb = mCurrentFrame.pixel(x, y);
                    int r = lighter(qRed(rgb), 100+dr);
                    int g = lighter(qGreen(rgb), 100+dg);
                    int b = lighter(qBlue(rgb), 100+db);
                    img.setPixel(x, y, QColor(r, g, b).lighter(100-dl).rgb());
                }
            }
        }
        else {
            if (img.height() != mFrameBrightness.count())
                return;
            for (int y = 0; y < mFrameBrightness.count(); ++y) {
                int dl = (int) ((mFrameBrightness[y] - mAvgBrightness) * lLevel);
                int dr = (int) ((mFrameRed[y] - mAvgRed) * rLevel);
                int dg = (int) ((mFrameGreen[y] - mAvgGreen) * gLevel);
                int db = (int) ((mFrameBlue[y] - mAvgBlue) * bLevel);
                for (int x = 0; x < img.width(); ++x) {
                    QRgb rgb = mCurrentFrame.pixel(x, y);
                    int r = lighter(qRed(rgb), 100+dr);
                    int g = lighter(qGreen(rgb), 100+dg);
                    int b = lighter(qBlue(rgb), 100+db);
                    img.setPixel(x, y, QColor(r, g, b).lighter(100-dl).rgb());
                }
            }
        }
        mPreviewForm->pictureWidget()->setPicture(img, -1);
    }
    else
        mPreviewForm->pictureWidget()->setPicture(mCurrentFrame, -1);
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
}


void MainWindow::resetRGBL(void)
{
    mPreviewForm->redSlider()->setValue(0);
    mPreviewForm->greenSlider()->setValue(0);
    mPreviewForm->blueSlider()->setValue(0);
    mPreviewForm->brightnessSlider()->setValue(0);
    deflicker();
}


void MainWindow::enableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(true);
    ui->frameTimeLineEdit->setEnabled(true);
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
    ui->actionAutofitPreview->setEnabled(true);
}


void MainWindow::disableGuiButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(false);
    ui->frameTimeLineEdit->setEnabled(false);
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
    ui->actionAutofitPreview->setEnabled(false);
}


void MainWindow::enablePreviewForm(void)
{
    mPreviewForm->brightnessSlider()->setEnabled(true);
    mPreviewForm->redSlider()->setEnabled(true);
    mPreviewForm->greenSlider()->setEnabled(true);
    mPreviewForm->blueSlider()->setEnabled(true);
    ui->actionCopy_to_clipboard->setEnabled(true);
}


void MainWindow::disablePreviewForm(void)
{
    mPreviewForm->brightnessSlider()->setEnabled(false);
    mPreviewForm->redSlider()->setEnabled(false);
    mPreviewForm->greenSlider()->setEnabled(false);
    mPreviewForm->blueSlider()->setEnabled(false);
    ui->actionCopy_to_clipboard->setEnabled(false);
}


void MainWindow::updateButtons(void)
{
    ui->AButton->setChecked(mProject.markA() != Project::INVALID_FRAME);
    ui->BButton->setChecked(mProject.markB() != Project::INVALID_FRAME);
}


void MainWindow::about(void)
{
    QMessageBox::about(this, tr("About %1").arg(MainWindow::AppName),
        tr("<p><strong>%1</strong> &ndash; Generate synchroballistic photography alike images from footage.</p>"
           "<p>Copyright (c) 2011 Oliver Lau &lt;oliver@von-und-fuer-lau.de&gt;</p>"
           "<p>VideoDecoder Copyright (c) 2009-2010 by Daniel Roggen &lt;droggen@gmail.com&gt;</p>"
           "<p>All rights reserved.</p>").arg(MainWindow::AppName));
}


void MainWindow::help(void)
{
    QMessageBox::information(this, tr("Help on %1").arg(MainWindow::AppName),
        tr("<p>Not implemented yet</p>"));
}


void MainWindow::openRecentProjectFile(void)
{
    QAction* action = qobject_cast<QAction *>(sender());
    if (action)
        openProject(action->data().toString());
}


void MainWindow::openProject(const QString& fileName)
{
    setCurrentProjectFile(fileName);
    mProject.load(fileName);
    mVideoReaderThread->setHistogramRegion(mProject.histogramRegion());
    mVideoWidget->setStripePos(mProject.stripePos());
    mVideoWidget->setStripeOrientation(mProject.stripeIsVertical());
    mVideoWidget->setHistogramRegion(mProject.histogramRegion());
    mPreviewForm->brightnessSlider()->setValue(mProject.brightnessLevel()*100);
    mPreviewForm->redSlider()->setValue(mProject.redLevel()*100);
    mPreviewForm->greenSlider()->setValue(mProject.greenLevel()*100);
    mPreviewForm->blueSlider()->setValue(mProject.blueLevel()*100);
    if (mProject.markAIsSet() && mProject.markBIsSet())
        ui->infoPlainTextEdit->appendPlainText(tr("%1 frames selected").arg(mProject.markB() - mProject.markA()));
    updateButtons();
    if (!mProject.videoFileName().isNull())
        loadVideoFile();
    if (mProject.currentFrame() != Project::INVALID_FRAME)
        mFrameSlider->setValue(mProject.currentFrame());
}


void MainWindow::openProject(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open project file"));
    if (fileName.isNull())
        return;
    openProject(fileName);
}


void MainWindow::saveProject(void)
{
    if (mProject.fileName().isEmpty()) {
        saveProjectAs();
    }
    else {
        mProject.save();
        ui->statusBar->showMessage(tr("Project saved."), 5000);
    }
}


void MainWindow::saveProjectAs(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save project file as ..."));
    if (fileName.isNull())
        return;
    mProject.save(fileName);
    setCurrentProjectFile(fileName);
    ui->statusBar->showMessage(tr("Project saved."), 5000);
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
    if (numRecentFiles > 0)
        ui->menuOpen_recent_project->setEnabled(true);
}


void MainWindow::openVideoFile(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open video file"));
    if (fileName.isNull())
        return;
    setCurrentVideoFile(fileName);
    loadVideoFile();
    clearMarks();
}


void MainWindow::loadVideoFile(void)
{
    bool ok = mVideoReaderThread->setFile(mProject.videoFileName());
    if (!ok)
        return; // TODO: display alert dialog
    mVideoWidget->setFrameSize(mVideoReaderThread->decoder()->frameSize());
    ui->action_CloseVideoFile->setEnabled(true);
    mCurrentFrame = QImage(mVideoReaderThread->decoder()->frameSize(), QImage::Format_RGB888);
    mCurrentFrame.fill(qRgb(116, 214, 252));
    mPreviewForm->pictureWidget()->resize(mVideoReaderThread->decoder()->frameSize());
    if (mProject.stripeIsVertical())
        mPreviewForm->setSizeConstraint(QSize(0, mVideoReaderThread->decoder()->frameSize().height()), QSize(QWIDGETSIZE_MAX, mVideoReaderThread->decoder()->frameSize().height()));
    else
        mPreviewForm->setSizeConstraint(QSize(mVideoReaderThread->decoder()->frameSize().width(), 0), QSize(mVideoReaderThread->decoder()->frameSize().width(), QWIDGETSIZE_MAX));
    mPreviewForm->pictureWidget()->setPicture(QImage(), -1);
    showPictureWidget();
    QImage img;
    ui->statusBar->showMessage(tr("Seeking last frame ..."));
    mVideoReaderThread->decoder()->seekMs(mVideoReaderThread->decoder()->getVideoLengthMs());
    mVideoReaderThread->decoder()->getFrame(img, &mLastFrameNumber);
    Q_ASSERT(mLastFrameNumber > 0);
    mFrameSlider->setMaximum(mLastFrameNumber);
    mFrameSlider->setValue(0);
    ui->infoPlainTextEdit->clear();
    ui->infoPlainTextEdit->appendPlainText(QString("%1 (%2)").arg(mVideoReaderThread->decoder()->formatCtx()->iformat->long_name).arg(mVideoReaderThread->decoder()->codec()->long_name));
    ui->infoPlainTextEdit->appendPlainText(tr("Last frame # %1").arg(mLastFrameNumber));
    ui->infoPlainTextEdit->appendPlainText(tr("Video length: %1").arg(ms2hmsz(mVideoReaderThread->decoder()->getVideoLengthMs(), false)));
    ui->actionSave_project->setEnabled(true);
    ui->actionSave_project_as->setEnabled(true);
    seekToFrame(0);
    enableGuiButtons();
    ui->statusBar->showMessage(tr("Ready."), 2000);
}


void MainWindow::closeVideoFile(void)
{
    disableGuiButtons();
    ui->frameNumberLineEdit->setText(QString());
    ui->frameTimeLineEdit->setText(QString());
    mFrameSlider->setValue(0);
    mVideoWidget->setFrame(QImage(), Histogram(), -1);
    mPreviewForm->pictureWidget()->setPicture(QImage(), -1);
    hidePictureWidget();
    clearMarks();
    ui->statusBar->showMessage(tr("File closed."), 5000);
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png, *.jpg");
    mPreviewForm->pictureWidget()->picture().save(saveFileName);
}


void MainWindow::autoFitPreview(void)
{
    mPreviewForm->resize(mVideoReaderThread->decoder()->frameSize()); // XXX
}
