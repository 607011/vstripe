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

const QString MainWindow::Company = "von-und-fuer-lau.de";
const QString MainWindow::AppName = "VStripe";


MainWindow::MainWindow(int argc, char* argv[], QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mVideoWidget = new VideoWidget;
    ui->verticalLayout->insertWidget(0, mVideoWidget);
    connect(mVideoWidget, SIGNAL(fileDropped(QString)), this, SLOT(fileDropped(QString)));
    connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), this, SLOT(setStripeOrientation(bool)));
    connect(mVideoWidget, SIGNAL(stripeOrientationChanged(bool)), &mProject, SLOT(setStripeOrientation(bool)));
    connect(mVideoWidget, SIGNAL(stripePosChanged(int)), &mProject, SLOT(setStripePos(int)));

    ui->AButton->setStyleSheet("background: green");
    ui->BButton->setStyleSheet("background: red");

    connect(ui->action_OpenVideoFile, SIGNAL(triggered()), this, SLOT(openVideoFile()));
    connect(ui->action_CloseVideoFile, SIGNAL(triggered()), this, SLOT(closeVideoFile()));
    connect(ui->actionAutofitPreview, SIGNAL(triggered()), this, SLOT(autoFitPreview()));

    mFrameSlider = new MarkableSlider(&mProject);
    mFrameSlider->setEnabled(false);
    ui->sliderLayout->insertWidget(0, mFrameSlider);
    connect(mFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(seekToFrame(int)));
    connect(mFrameSlider, SIGNAL(valueChanged(int)), &mProject, SLOT(setCurrentFrame(int)));

    mVideoReaderThread = new VideoReaderThread;
    connect(mVideoReaderThread, SIGNAL(finished()), this, SLOT(decodingFinished()));
    connect(mVideoReaderThread, SIGNAL(percentReady(int)), this, SLOT(showPercentReady(int)));
    connect(mVideoReaderThread, SIGNAL(frameReady(QImage,int,int,int)), this, SLOT(frameReady(QImage,int,int,int)), Qt::QueuedConnection);
    connect(mVideoReaderThread, SIGNAL(frameReady(QImage,int,int,int)), mVideoWidget, SLOT(setFrame(QImage)), Qt::QueuedConnection);

    mPictureWidget = new PictureWidget;
    connect(ui->actionPreview_picture, SIGNAL(toggled(bool)), this, SLOT(togglePictureWidget(bool)));
    if (ui->actionPreview_picture->isChecked())
        mPictureWidget->show();
    connect(mPictureWidget, SIGNAL(visibilityChanged(bool)), ui->actionPreview_picture, SLOT(setChecked(bool)));
    connect(mPictureWidget, SIGNAL(sizeChanged(const QSize&)), this, SLOT(setPictureSize(const QSize&)));

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
    delete mPictureWidget;
    delete mFrameSlider;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    saveAppSettings();
    QMainWindow::closeEvent(event);
    mPictureWidget->close();
}


void MainWindow::changeEvent(QEvent* event)
{
//    if (event->spontaneous() && isVisible() && event->type() == QEvent::ActivationChange) {
//        if (!mFileNameFromCmdLine.isEmpty()) {
//            fileDropped(mFileNameFromCmdLine); // TODO
//            mFileNameFromCmdLine.clear();
//        }
//    }
    QWidget::changeEvent(event);
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
    updateRecentProjectFileActions();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        ui->menuOpen_recent_video->addAction(recentVideoFileActs[i]);
        ui->menuOpen_recent_project->addAction(recentProjectFileActs[i]);
    }
}


void MainWindow::setCurrentVideoFile(const QString& fileName)
{
    mProject.setVideoFileName(fileName);
    setWindowTitle(tr("%1 - %2").arg(MainWindow::AppName).arg(mProject.videoFileName()));
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
    QImage img;
    ui->statusBar->showMessage(tr("Seeking to frame #%1 ...").arg(n), 3000);
    mVideoReaderThread->decoder()->seekFrame(n);
    mVideoReaderThread->decoder()->getFrame(img, &mEffectiveFrameNumber, &mEffectiveFrameTime, &mDesiredFrameNumber, &mDesiredFrameTime);
    qDebug() << QString("effective: %1 (%2 ms), desired: %3 (%4 ms)").arg(mEffectiveFrameNumber).arg(mEffectiveFrameTime).arg(mDesiredFrameNumber).arg(mDesiredFrameTime);
    mVideoWidget->setFrame(img);
    ui->frameNumberLineEdit->setText(QString("%1").arg(mEffectiveFrameNumber));
    ui->frameTimeLineEdit->setText(ms2hmsz(mEffectiveFrameTime));
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
        mPictureWidget->setSizeConstraint(QSize(0, mVideoReaderThread->decoder()->frameSize().height()), QSize(QWIDGETSIZE_MAX, mVideoReaderThread->decoder()->frameSize().height()));
    else
        mPictureWidget->setSizeConstraint(QSize(mVideoReaderThread->decoder()->frameSize().width(), 0), QSize(mVideoReaderThread->decoder()->frameSize().width(), QWIDGETSIZE_MAX));
    mPictureWidget->resize(mVideoReaderThread->decoder()->frameSize());
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
    mProject.setFixed(mVideoWidget->stripeIsFixed());
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


void MainWindow::frameReady(QImage src, int frameNumber, int effectiveFrameNumber, int effectiveFrameTime)
{
    Q_ASSERT(!mCurrentFrame.isNull());
    const int srcpos = mProject.stripeIsFixed()? mVideoWidget->stripePos() : (frameNumber % (mVideoWidget->stripeIsVertical()? src.width() : src.height()));
    const int dstpos = frameNumber;
    if (mVideoWidget->stripeIsVertical()) {
        for (int y = 0; y < src.height(); ++y)
            mCurrentFrame.setPixel(dstpos, y, src.pixel(srcpos, y));
    }
    else {
        for (int x = 0; x < src.width(); ++x)
            mCurrentFrame.setPixel(x, dstpos, src.pixel(x, srcpos));
    }
    mPictureWidget->setPicture(mCurrentFrame);
    mFrameSlider->blockSignals(true);
    mFrameSlider->setValue(mPreRenderFrameNumber + int(frameNumber * mFrameDelta));
    mFrameSlider->blockSignals(false);
    ui->frameNumberLineEdit->setText(QString("%1").arg(effectiveFrameNumber));
    ui->frameTimeLineEdit->setText(ms2hmsz(effectiveFrameTime));
}


void MainWindow::decodingFinished()
{
    ui->action_Save_picture->setEnabled(true);
    ui->renderButton->setText(tr("Start rendering"));
    mPreRenderFrameNumber = mFrameSlider->value();
    mProject.setCurrentFrame(mPreRenderFrameNumber);
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
    if (!mProject.videoFileName().isNull())
        loadVideoFile();
    if (mProject.currentFrame() != Project::INVALID_FRAME)
        mFrameSlider->setValue(mProject.currentFrame());
    mVideoWidget->setStripePos(mProject.stripePos());
    mVideoWidget->setStripeOrientation(mProject.stripeIsVertical());
    if (mProject.markAIsSet() && mProject.markBIsSet())
        ui->infoPlainTextEdit->appendPlainText(tr("%1 frames selected").arg(mProject.markB() - mProject.markA()));
    updateButtons();
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
    mPictureWidget->resize(mVideoReaderThread->decoder()->frameSize());
    if (mProject.stripeIsVertical())
        mPictureWidget->setSizeConstraint(QSize(0, mVideoReaderThread->decoder()->frameSize().height()), QSize(QWIDGETSIZE_MAX, mVideoReaderThread->decoder()->frameSize().height()));
    else
        mPictureWidget->setSizeConstraint(QSize(mVideoReaderThread->decoder()->frameSize().width(), 0), QSize(mVideoReaderThread->decoder()->frameSize().width(), QWIDGETSIZE_MAX));
    mPictureWidget->setPicture(QImage());
    showPictureWidget();
    QImage img;
    ui->statusBar->showMessage(tr("Seeking last frame ..."));
    mVideoReaderThread->decoder()->seekMs(mVideoReaderThread->decoder()->getVideoLengthMs());
    mVideoReaderThread->decoder()->getFrame(img, &mLastFrameNumber);
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
    mVideoWidget->setFrame(QImage());
    mPictureWidget->setPicture(QImage());
    hidePictureWidget();
    clearMarks();
    ui->statusBar->showMessage(tr("File closed."), 5000);
}


void MainWindow::savePicture(void)
{
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save picture as ..."), QString(), "*.png, *.jpg");
    mPictureWidget->picture().save(saveFileName);
}


void MainWindow::autoFitPreview(void)
{
    mPictureWidget->resize(mVideoReaderThread->decoder()->frameSize()); // XXX
}
