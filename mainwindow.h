/* Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>
 * All rights reserved.
 * $Id$
 */

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
#include <QString>
#include <QVector>

#include "videoreaderthread.h"
#include "videowidget.h"
#include "picturewidget.h"
#include "markableslider.h"
#include "project.h"
#include "previewform.h"
#include "histogram.h"

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int argc, char* argv[], QWidget* parent = NULL);
    ~MainWindow();

    QSize minimumSizeHint(void) const { return QSize(720, 576); }
    QSize sizeHint(void) const { return QSize(1280, 720); }

    static const QString Company;
    static const QString AppName;
    static const QString AppVersion;


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
    void clearHistogramRegion(void);
    void jumpToPrevMark(void);
    void jumpToNextMark(void);
    void savePicture(void);
    void showPercentReady(int);
    void frameReady(const QImage&, const Histogram&, int, int, int);
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
    void deflicker(void);
    void resetRGBL(void);

protected:
    void closeEvent(QCloseEvent*);

private: // variables
    Ui::MainWindow* ui;
    MarkableSlider* mFrameSlider;
    VideoWidget* mVideoWidget;
    PreviewForm* mPreviewForm;
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

    BrightnessData mFrameBrightness;
    BrightnessData mFrameRed;
    BrightnessData mFrameGreen;
    BrightnessData mFrameBlue;
    qreal mAvgBrightness;
    qreal mAvgRed;
    qreal mAvgGreen;
    qreal mAvgBlue;
    qreal mMinTotalBrightness;
    qreal mMinTotalRed;
    qreal mMinTotalGreen;
    qreal mMinTotalBlue;

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
    void enablePreviewForm(void);
    void disablePreviewForm(void);
    void updateButtons(void);
    void restoreAppSettings(void);
    void saveAppSettings(void);
    void updateRecentVideoFileActions(void);
    void updateRecentProjectFileActions(void);
    void setCurrentVideoFile(const QString&);
    void setCurrentProjectFile(const QString&);

    static QString ms2hmsz(int ms, bool withMs = true);
    static QString strippedName(const QString& fullFileName);
    static qreal averageBrightnessData(const BrightnessData&);
    static int darker(int v, int factor);
    static int lighter(int v, int factor);
};

#endif // __MAINWINDOW_H_

