/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>

#include "videoreaderthread.h"
#include "videowidget.h"
#include "picturewidget.h"
#include "markableslider.h"
#include "project.h"

namespace Ui {
    class MainWindow;
}

typedef QVector<qreal> Histogram;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char* argv[], QWidget* parent = NULL);
    ~MainWindow();

    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    QSize sizeHint(void) const { return QSize(1280, 720); }

    static const QString Company;
    static const QString AppName;

    static const int HistogramBinCount = 256;

public slots:
    void openVideoFile(void);
    void closeVideoFile(void);
    void openRecentVideoFile(void);
    void openRecentProjectFile(void);
    void loadVideoFile(void);
    void decodingFinished(void);
    void togglePictureWidget(bool);
    void seekToFrame(int);
    void forward(int nFrames);
    void backward(int nFrames);
    void forward(void) { forward(1); }
    void backward(void) { backward(1); }
    void fastForward(void);
    void fastBackward(void);
    void setMarkA(void);
    void setMarkB(void);
    void setMark(void);
    void clearMarks(void);
    void jumpToPrevMark(void);
    void jumpToNextMark(void);
    void savePicture(void);
    void showPercentReady(int);
    void frameReady(QImage, int, int, int);
    void renderButtonClicked(void);
    void setParamsButtonClicked(void);
    void about(void);
    void help(void);
    void saveProject(void);
    void saveProjectAs(void);
    void openProject(void);
    void openProject(const QString&);
    void fileDropped(const QString&);
    void setPictureSize(const QSize&);
    void setStripeOrientation(bool vertical);
    void autoFitPreview(void);

protected:
    void closeEvent(QCloseEvent*);
    void changeEvent(QEvent*);

private: // variables
    Ui::MainWindow* ui;
    MarkableSlider* mFrameSlider;
    VideoWidget* mVideoWidget;
    PictureWidget* mPictureWidget;
    VideoReaderThread* mVideoReaderThread;
    qreal mFrameDelta;
    int mFrameCount;
    QImage mCurrentFrame;
    int mLastFrameNumber;
    int mEffectiveFrameNumber;
    int mEffectiveFrameTime;
    int mDesiredFrameNumber;
    int mDesiredFrameTime;
    int mPreRenderFrameNumber;
    QString mFileNameFromCmdLine;
    QVector<qreal> mFrameBrightness;
    qreal mHistogram[HistogramBinCount];

    Project mProject;

    static const int MaxRecentFiles = 16;
    QAction* recentVideoFileActs[MaxRecentFiles];
    QAction* recentProjectFileActs[MaxRecentFiles];

private: // methods
    void showPictureWidget(void);
    void hidePictureWidget(void);
    void startRendering(void);
    void stopRendering(void);
    void enableGuiButtons(void);
    void disableGuiButtons(void);
    void updateButtons(void);
    void restoreAppSettings(void);
    void saveAppSettings(void);
    void updateRecentVideoFileActions(void);
    void updateRecentProjectFileActions(void);
    void setCurrentVideoFile(const QString&);
    void setCurrentProjectFile(const QString&);
    qreal calculateHistogram(void);

    static QString ms2hmsz(int ms, bool withMs = true);
    static QString strippedName(const QString& fullFileName);
};

#endif // MAINWINDOW_H

