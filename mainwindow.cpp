/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#include <omp.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QtDebug>
#include <QImage>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <QSettings>
#include <QTextStream>
#include <QFileInfo>
#include <QPainter>

#include <QMap>
#include <QUrl>
#ifdef WITH_HELPBROWSER
#include <QtHelp/QHelpEngineCore>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "histogram.h"
#include "webcam.h"
#include "videodecoder.h"
#include "webcamthread.h"


const QString MainWindow::Company = "von-und-fuer-lau.de";
const QString MainWindow::AppName = "VStripe";
#ifndef QT_NO_DEBUG
const QString MainWindow::AppVersion = "0.9.8.4 DEBUG $Date$";
#else
const QString MainWindow::AppVersion = "0.9.8.4";
#endif


MainWindow::MainWindow(int argc, char* argv[], QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
#ifdef WITH_HELPBROWSER
        mHelpBrowser(NULL),
#endif
        mWebcamThread(NULL),
        mDecoder(NULL)
{
    ui->setupUi(this);

    setWindowTitle(tr("%1 %2").arg(MainWindow::AppName).arg(MainWindow::AppVersion));

    qRegisterMetaType<Histogram>();

    mVideoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, mVideoWidget);
    QObject::connect(mVideoWidget, SIGNAL(fileDropped(QString)), this, SLOT(fileDropped(QString)));
    QObject::connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), this, SLOT(setStripeOrientation(bool)));
    QObject::connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), &mProject, SLOT(setStripeOrientation(bool)));
    QObject::connect(mVideoWidget, SIGNAL(stripePosChanged(int)), &mProject, SLOT(setStripePos(int)));

    QObject::connect(ui->actionOpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    QObject::connect(ui->actionCloseInput, SIGNAL(triggered()), this, SLOT(closeInput()));
    QObject::connect(ui->actionHistogram, SIGNAL(toggled(bool)), mVideoWidget, SLOT(setHistogramEnabled(bool)));
    QObject::connect(ui->actionClear_histogram_region, SIGNAL(triggered()), this, SLOT(clearHistogramRegion()));

    mFrameSlider = new MarkableSlider(&mProject);
    mFrameSlider->setEnabled(false);
    ui->sliderLayout->insertWidget(0, mFrameSlider);
    QObject::connect(mFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(seekToFrame(int)));

    mVideoReaderThread = new VideoReaderThread(this);
    QObject::connect(mVideoReaderThread, SIGNAL(finished()), this, SLOT(decodingFinished()));
    QObject::connect(mVideoReaderThread, SIGNAL(percentReady(int)), this, SLOT(showPercentReady(int)));
    QObject::connect(mVideoReaderThread, SIGNAL(frameReady(QImage,Histogram,int,int,int)), this, SLOT(frameReady(QImage,Histogram,int,int,int)));
    QObject::connect(mVideoWidget, SIGNAL(histogramRegionChanged(QRect)), mVideoReaderThread, SLOT(setHistogramRegion(QRect)));
    QObject::connect(mVideoWidget, SIGNAL(histogramRegionChanged(QRect)), &mProject, SLOT(setHistogramRegion(QRect)));

    mPreviewForm = new PreviewForm;
    QObject::connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        mPreviewForm->show();
    QObject::connect(mPreviewForm, SIGNAL(correctionsChanged()), this, SLOT(deflicker()));
    QObject::connect(mPreviewForm, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));
    QObject::connect(mPreviewForm, SIGNAL(pictureSizeChanged(QSize)), this, SLOT(setPictureSize(QSize)));
    QObject::connect(mPreviewForm->brightnessSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    QObject::connect(mPreviewForm->redSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    QObject::connect(mPreviewForm->greenSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    QObject::connect(mPreviewForm->blueSlider(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));
    QObject::connect(mPreviewForm->factorDial(), SIGNAL(sliderReleased()), this, SLOT(deflicker()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentVideoFileActs[i] = new QAction(this);
        recentVideoFileActs[i]->setVisible(false);
        QObject::connect(recentVideoFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentVideoFile()));
        recentProjectFileActs[i] = new QAction(this);
        recentProjectFileActs[i]->setVisible(false);
        QObject::connect(recentProjectFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentProjectFile()));
    }

    QObject::connect(ui->forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    QObject::connect(ui->backwardButton, SIGNAL(clicked()), this, SLOT(backward()));
    QObject::connect(ui->fastForwardButton, SIGNAL(clicked()), this, SLOT(fastForward()));
    QObject::connect(ui->fastBackwardButton, SIGNAL(clicked()), this, SLOT(fastBackward()));
    QObject::connect(ui->setParamsButton, SIGNAL(clicked()), this, SLOT(setParamsButtonClicked()));
    QObject::connect(ui->renderButton, SIGNAL(clicked()), this, SLOT(renderButtonClicked()));
    QObject::connect(ui->action_Save_picture, SIGNAL(triggered()), this, SLOT(savePicture()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    QObject::connect(ui->actionOpen_project, SIGNAL(triggered()), this, SLOT(openProject()));
    QObject::connect(ui->actionSave_project, SIGNAL(triggered()), this, SLOT(saveProject()));
    QObject::connect(ui->actionSave_project_as, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
    QObject::connect(ui->actionCopy_to_clipboard, SIGNAL(triggered()), mPreviewForm->pictureWidget(), SLOT(copyImageToClipboard()));

    mEffectiveFrameNumber = Project::INVALID_FRAME;
    mEffectiveFrameTime = -1;
    mPreRenderFrameNumber = 0;
    mProject.setCurrentFrame(0);
    QObject::connect(ui->AButton, SIGNAL(clicked()), this, SLOT(setMarkA()));
    QObject::connect(ui->BButton, SIGNAL(clicked()), this, SLOT(setMarkB()));
    QObject::connect(ui->markButton, SIGNAL(clicked()), this, SLOT(setMark()));
    QObject::connect(ui->actionClear_marks, SIGNAL(triggered()), this, SLOT(clearMarks()));
    QObject::connect(ui->prevMarkButton, SIGNAL(clicked()), this, SLOT(jumpToPrevMark()));
    QObject::connect(ui->nextMarkButton, SIGNAL(clicked()), this, SLOT(jumpToNextMark()));

    // generate menu item for each attached webcam
    QMenu* webcamMenu = NULL;
    for (int i = 0; /* */; ++i) {
        Webcam cam;
        if (!cam.open(i))
            break;
        QImage img;
        bool ok = cam.getFrame(img);
        if (!ok || img.isNull()) // TODO: works with Windows, but with Mac OS X (10.6) there is no error despite an invalid camera id; not tested with Linux
            break;
        if (webcamMenu == NULL)
            webcamMenu = new QMenu(tr("Open webcam"), ui->menu_File);
        QAction* camMenu = new QAction(QString("#%1 (%2x%3)").arg(i).arg(cam.frameSize().width()).arg(cam.frameSize().height()), webcamMenu);
        camMenu->setData(i);
        webcamMenu->addAction(camMenu);
        QObject::connect(camMenu, SIGNAL(triggered()), this, SLOT(openWebcam()));
        break; // one camera is enough, use it (TODO: remove this statement if above TODO is solved)
    }
    if (webcamMenu)
        ui->menu_File->insertMenu(ui->actionOpenVideoFile, webcamMenu);

    restoreAppSettings();

    if (argc > 1)
        mFileNameFromCmdLine = argv[1];

    ui->statusBar->showMessage(tr("Ready."), 3000);
}


MainWindow::~MainWindow()
{
    if (mWebcamThread)
        delete mWebcamThread;
    if (mDecoder)
        delete mDecoder;
#ifdef WITH_HELPBROWSER
    if (mHelpBrowser)
        delete mHelpBrowser;
#endif
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
    else
        QMessageBox::warning(this, tr("Invalid file"), tr("File '%1' does not exist or is not accessible").arg(fileName), QMessageBox::Ok);
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
            QMessageBox::critical(this, tr("File does not exist"), tr("File '%1' does not exist").arg(fileName), QMessageBox::Ok);
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


void MainWindow::deactivateWebcam(void)
{
    if (mWebcamThread) {
        delete mWebcamThread;
        mWebcamThread = NULL;
    }
    if (mDecoder) {
        delete mDecoder;
        mDecoder = NULL;
    }
}


bool MainWindow::webcamIsActive(void) const
{
    return mWebcamThread != NULL;
}


QSize MainWindow::optimalPictureSize(void) const
{
    QSize goalSize(mDecoder->frameSize());
    if (mProject.markAIsSet() && mProject.markBIsSet()) {
        if (mProject.stripeIsVertical())
            goalSize.setWidth(mProject.markB() - mProject.markA());
        else
            goalSize.setHeight(mProject.markB() - mProject.markA());
    }
    else {
        if (mProject.stripeIsVertical())
            goalSize.setWidth(webcamIsActive()? 9999: mLastFrameNumber);
        else
            goalSize.setHeight(webcamIsActive()? 9999: mLastFrameNumber);
    }
    return goalSize;
}


void MainWindow::seekToFrame(int n)
{
    setCursor(Qt::WaitCursor);
    mPreviewForm->setCursor(Qt::WaitCursor);
    mProject.setCurrentFrame(n);
    ui->statusBar->showMessage(tr("Seeking to frame #%1 ...").arg(n));
    mDecoder->seekFrame(n);
    QImage img;
    mDecoder->getFrame(img, &mEffectiveFrameNumber, &mEffectiveFrameTime, &mDesiredFrameNumber, &mDesiredFrameTime);
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
    ui->statusBar->showMessage(tr("Loading %1 frames ... %2%").arg(mFrameCount).arg(percent));
}


void MainWindow::setStripeOrientation(bool vertical)
{
    if (mStripeImage.isNull())
        return;
    mProject.setStripeOrientation(vertical);
    if (vertical)
        mPreviewForm->setSizeConstraints(QSize(0, mDecoder->frameSize().height()), optimalPictureSize(), mDecoder->frameSize());
    else
        mPreviewForm->setSizeConstraints(QSize(mDecoder->frameSize().width(), 0), optimalPictureSize(), mDecoder->frameSize());
}


void MainWindow::setPictureSize(const QSize& size)
{
    qDebug() << "MainWindow::setPictureSize(" << size << ")";
    if (mVideoReaderThread->isRunning())
        stopRendering();
    mStripeImage = QImage(size, QImage::Format_RGB888);
    if (!mStripeImage.isNull()) {
        mStripeImage.fill(qRgb(82, 179, 169));
        QImage bg(QString(":/images/checkered-background.png"));
        QPainter painter(&mStripeImage);
        painter.setBrush(QBrush(bg.convertToFormat(QImage::Format_RGB888)));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, size.width(), size.height());
    }
    mPreviewForm->pictureWidget()->setPicture(mStripeImage, -1);
    mPreviewForm->setPictureSize(size);
}


void MainWindow::startRendering(void)
{
    if (webcamIsActive())
        mWebcamThread->stopReading();
    ui->renderButton->setText(tr("Stop rendering"));
    disablePreviewForm();
    disableNavigationButtons();
    mFrameBrightness.clear();
    mFrameRed.clear();
    mFrameGreen.clear();
    mFrameBlue.clear();
    mMinTotalBrightness = INT_MAX;
    mMinTotalRed = INT_MAX;
    mMinTotalGreen = INT_MAX;
    mMinTotalBlue = INT_MAX;
    mPreviewForm->pictureWidget()->setBrightnessData(&mFrameBrightness, &mFrameRed, &mFrameGreen, &mFrameBlue);

    mFrameCount = mProject.stripeIsVertical()? mStripeImage.width() : mStripeImage.height();
    int firstFrame = 0;
    if (mDecoder->typeName() == "VideoFile") {
        if (mProject.markA() != Project::INVALID_FRAME && mProject.markB() != Project::INVALID_FRAME && mProject.markB() <= mProject.markA()) {
            QMessageBox::information(this, tr("Invalid markers"), tr("Marker B must be after marker A"), QMessageBox::Ok);
            return;
        }
        if (mProject.markA() == Project::INVALID_FRAME) {
            mProject.setMarkA(0);
            ui->AButton->setChecked(true);
        }
        if (mProject.markB() == Project::INVALID_FRAME) {
            mProject.setMarkB(mLastFrameNumber);
            ui->BButton->setChecked(true);
        }
        mFrameSlider->setValue(mProject.markA());
        mFrameDelta = (qreal)(mProject.markB() - mProject.markA()) / mFrameCount;
        mPreRenderFrameNumber = mFrameSlider->value();
        mProject.setCurrentFrame(mPreRenderFrameNumber);
        firstFrame = mProject.markA();
    }
    else { // mDecoder->typeName() == "Webcam"
        mFrameDelta = 1;
    }
    ui->statusBar->showMessage(tr("Loading %1 frames ...").arg(mFrameCount));
    mVideoReaderThread->startReading(firstFrame, mFrameCount, mFrameDelta);
}


void MainWindow::stopRendering(void) {
    ui->statusBar->showMessage(tr("Rendering stopped."), 5000);
    ui->renderButton->setText(tr("Start rendering"));
    mVideoReaderThread->stopReading();
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
    enableNavigationButtons();
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
    mFrameSlider->setValue(mFrameSlider->value() + nFrames * mDecoder->getDefaultSkip());
}


void MainWindow::backward(int nFrames)
{
    mFrameSlider->setValue(mFrameSlider->value() - nFrames * mDecoder->getDefaultSkip());
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
    if (!mProject.marks().contains(Project::mark_type(mEffectiveFrameNumber, QString()))) {
        mProject.appendMark(Project::mark_type(mEffectiveFrameNumber, QString()));
        mFrameSlider->update();
    }
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
    Q_ASSERT(!mStripeImage.isNull());
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
            mStripeImage.setPixel(dstpos, y, src.pixel(srcpos, y));
    }
    else {
        for (int x = 0; x < src.width(); ++x)
            mStripeImage.setPixel(x, dstpos, src.pixel(x, srcpos));
    }
    mPreviewForm->pictureWidget()->setPicture(mStripeImage, dstpos, mProject.stripeIsVertical());
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
    enableNavigationButtons();
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
#ifndef NODEBUG
    QTime t;
    t.start();
#endif
    setCursor(Qt::BusyCursor);
    mPreviewForm->setCursor(Qt::BusyCursor);
    qreal f = 1e-2 * 14 * mPreviewForm->amplificationCorrection();
    qreal lLevel = f * mPreviewForm->brightnessSlider()->value();
    qreal rLevel = f * mPreviewForm->redSlider()->value();
    qreal gLevel = f * mPreviewForm->greenSlider()->value();
    qreal bLevel = f * mPreviewForm->blueSlider()->value();
    mProject.setBrightnessLevel(lLevel);
    mProject.setRedLevel(rLevel);
    mProject.setGreenLevel(gLevel);
    mProject.setBlueLevel(bLevel);
    if (lLevel > 0 || rLevel != 0 || gLevel != 0 || bLevel != 0) {
        QImage img(mStripeImage.size(), mStripeImage.format());
        if (mVideoWidget->stripeIsVertical()) {
            if (img.width() != mFrameBrightness.count())
                return;
#pragma omp parallel for
            for (int x = 0; x < mFrameBrightness.count(); ++x) {
                int dl = (int) ((mFrameBrightness[x] - mAvgBrightness) * lLevel);
                int dr = (int) ((mFrameRed[x] - mAvgRed) * rLevel);
                int dg = (int) ((mFrameGreen[x] - mAvgGreen) * gLevel);
                int db = (int) ((mFrameBlue[x] - mAvgBlue) * bLevel);
                for (int y = 0; y < img.height(); ++y) {
                    QRgb rgb = mStripeImage.pixel(x, y);
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
#pragma omp parallel for
            for (int y = 0; y < mFrameBrightness.count(); ++y) {
                int dl = (int) ((mFrameBrightness[y] - mAvgBrightness) * lLevel);
                int dr = (int) ((mFrameRed[y] - mAvgRed) * rLevel);
                int dg = (int) ((mFrameGreen[y] - mAvgGreen) * gLevel);
                int db = (int) ((mFrameBlue[y] - mAvgBlue) * bLevel);
                for (int x = 0; x < img.width(); ++x) {
                    QRgb rgb = mStripeImage.pixel(x, y);
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
        mPreviewForm->pictureWidget()->setPicture(mStripeImage, -1);
    setCursor(Qt::ArrowCursor);
    mPreviewForm->setCursor(Qt::ArrowCursor);
}


void MainWindow::enableNavigationButtons(void)
{
    ui->frameNumberLineEdit->setEnabled(true);
    ui->frameTimeLineEdit->setEnabled(true);
    ui->setParamsButton->setEnabled(true);
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
    ui->actionCloseInput->setEnabled(true);
    ui->action_Save_picture->setEnabled(true);
    ui->actionClear_marks->setEnabled(true);
}


void MainWindow::enableGuiButtons(void)
{
    ui->renderButton->setEnabled(true);
    ui->actionCloseInput->setEnabled(true);
    ui->action_Save_picture->setEnabled(true);
    ui->actionClear_marks->setEnabled(true);
    enableNavigationButtons();
}


void MainWindow::disableNavigationButtons(void)
{

    ui->frameNumberLineEdit->setEnabled(false);
    ui->frameTimeLineEdit->setEnabled(false);
    ui->setParamsButton->setEnabled(false);
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
    ui->actionCloseInput->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
    ui->actionClear_marks->setEnabled(false);
}


void MainWindow::disableGuiButtons(void)
{
    ui->renderButton->setEnabled(false);
    ui->actionCloseInput->setEnabled(false);
    ui->action_Save_picture->setEnabled(false);
    ui->actionClear_marks->setEnabled(false);
    disableNavigationButtons();
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
        tr("<p><strong>%1</strong> &ndash; Generate streak photos from footage.</p>"
           "<p>Copyright (c) 2011 Oliver Lau &lt;oliver@von-und-fuer-lau.de&gt;</p>"
           "<p>VideoDecoder Copyright (c) 2009-2010 by Daniel Roggen &lt;droggen@gmail.com&gt;</p>"
#ifdef WITH_HELPBROWSER
           "<p>HelpBrowser Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).</p>"
#endif
           "<p>All rights reserved.</p>").arg(MainWindow::AppName));
}


void MainWindow::help(void)
{
#ifdef WITH_HELPBROWSER
//    if (mHelpBrowser == NULL)
//        mHelpBrowser = new HelpBrowser;
//    mHelpBrowser->showHelpForKeyword("VStripe::index");
//    mHelpBrowser->show();
#endif
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


void MainWindow::timerEvent(QTimerEvent*)
{
    /* ... */
}


IAbstractVideoDecoder* MainWindow::useDecoder(IAbstractVideoDecoder* decoder)
{
    deactivateWebcam();
    if (mDecoder)
        delete mDecoder;
    mDecoder = decoder;
    mVideoReaderThread->setSource(mDecoder);
    return mDecoder;
}


void MainWindow::openWebcam(void)
{
    QAction* action = qobject_cast<QAction*>(sender());
    int camId = action->data().toInt();
    useDecoder(new Webcam(this))->open(camId); // useDecoder() sets mDecoder
    mVideoReaderThread->setSource(mDecoder);
    mVideoWidget->setFrameSize(mDecoder->frameSize());
    setPictureSize(mDecoder->frameSize());
    mEffectiveFrameNumber = 0;
    mProject.setCurrentFrame(0);
    mWebcamThread = new WebcamThread(qobject_cast<Webcam*>(mDecoder), this);
    QObject::connect(mWebcamThread, SIGNAL(frameReady(QImage)), mVideoWidget, SLOT(setFrame(const QImage&)));
    if (mProject.stripeIsVertical())
        mPreviewForm->setSizeConstraints(QSize(0, mDecoder->frameSize().height()), optimalPictureSize(), mDecoder->frameSize());
    else
        mPreviewForm->setSizeConstraints(QSize(mDecoder->frameSize().width(), 0), optimalPictureSize(), mDecoder->frameSize());
    mPreviewForm->pictureWidget()->setPicture(QImage(), -1);
    showPictureWidget();
    QImage img;
    mDecoder->getFrame(img, &mLastFrameNumber);
    mFrameSlider->setEnabled(false);
    ui->infoPlainTextEdit->clear();
    ui->actionSave_project->setEnabled(true);
    ui->actionSave_project_as->setEnabled(true);
    enableGuiButtons();
    mWebcamThread->startReading();
    ui->statusBar->showMessage(tr("Webcam is running ..."));
    clearMarks();
}


void MainWindow::openVideoFile(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open video file"));
    if (fileName.isNull())
        return;
    setCurrentVideoFile(fileName);
    bool ok = loadVideoFile();
    if (ok)
        clearMarks();
}


bool MainWindow::loadVideoFile(void)
{
    bool ok = useDecoder(new VideoDecoder)->open(mProject.videoFileName().toLatin1().constData()); // useDecoder() sets mDecoder
    if (!ok) {
        mVideoWidget->setFrame(QImage());
        QMessageBox::warning(this, tr("Opening video failed."), tr("The selected video could not be read."), QMessageBox::Ok);
        ui->statusBar->showMessage(tr("Opening video failed."), 5000);
        return false;
    }
    mVideoWidget->setFrameSize(mDecoder->frameSize());
    ui->statusBar->showMessage(tr("Seeking last frame ..."));
    mDecoder->seekMs(mDecoder->getVideoLengthMs()-1);
    QImage img;
    mDecoder->getFrame(img, &mLastFrameNumber);
    if (mLastFrameNumber == 0) {
        mVideoWidget->setFrame(QImage());
        QMessageBox::warning(this, tr("Opening video failed."), tr("The selected video could not be read. Be sure to have any video encoded with H.264 and saved in AVI format."), QMessageBox::Ok);
        ui->statusBar->showMessage(tr("Opening video failed."), 5000);
        return false;
    }
    setPictureSize(mDecoder->frameSize());
    if (mProject.stripeIsVertical())
        mPreviewForm->setSizeConstraints(QSize(0, mDecoder->frameSize().height()), optimalPictureSize(), mDecoder->frameSize());
    else
        mPreviewForm->setSizeConstraints(QSize(mDecoder->frameSize().width(), 0), optimalPictureSize(), mDecoder->frameSize());
    showPictureWidget();
    ui->actionCloseInput->setEnabled(true);
    mFrameSlider->setMaximum(mLastFrameNumber);
    mFrameSlider->setValue(0);
    ui->infoPlainTextEdit->clear();
    ui->infoPlainTextEdit->appendPlainText(mDecoder->codecInfo());
    ui->infoPlainTextEdit->appendPlainText(tr("Last frame # %1").arg(mLastFrameNumber));
    ui->infoPlainTextEdit->appendPlainText(tr("Video length: %1").arg(ms2hmsz(mDecoder->getVideoLengthMs(), false)));
    ui->actionSave_project->setEnabled(true);
    ui->actionSave_project_as->setEnabled(true);
    seekToFrame(0);
    enableGuiButtons();
    ui->statusBar->showMessage(tr("Video is ready."));
    return true;
}


void MainWindow::closeInput(void)
{
    deactivateWebcam();
    disableGuiButtons();
    ui->frameNumberLineEdit->setText(QString());
    ui->frameTimeLineEdit->setText(QString());
    mFrameSlider->setValue(0);
    mVideoWidget->setFrame(QImage(), Histogram(), -1);
    mPreviewForm->pictureWidget()->setPicture(QImage(), -1);
    hidePictureWidget();
    clearMarks();
    ui->statusBar->showMessage(tr("Input closed."), 5000);
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png, *.jpg");
    mPreviewForm->pictureWidget()->picture().save(saveFileName, 0, 80);
}
