/* Copyright (c) 2011 Oliver Lau <oliver@ersatzworld.net>
 * All rights reserved.
 * $Id$
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QPair>

#include "videoreaderthread.h"
#include "videowidget.h"
#include "picturewidget.h"
#include "markableslider.h"
#include "project.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char* argv[], QWidget* parent = NULL);
    ~MainWindow();

    virtual QSize minimumSizeHint(void) const { return QSize(720, 576); }
    virtual QSize sizeHint(void) const { return QSize(1280, 720); }

    static const QString Company;
    static const QString AppName;

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
    void frameReady(QImage, int);
    void renderButtonClicked(void);
    void setParamsButtonClicked(void);
    void about(void);
    void help(void);
    void pictureWidthSet(int);
    void saveProject(void);
    void saveProjectAs(void);
    void openProject(void);
    void openProject(const QString&);
    void fileDropped(const QString&);

protected:
    void closeEvent(QCloseEvent*);

private: // variables
    Ui::MainWindow* ui;
    MarkableSlider* mFrameSlider;
    VideoWidget* mVideoWidget;
    PictureWidget* mPictureWidget;
    VideoReaderThread* mVideoReaderThread;
    qreal mFrameSkip; // so viel Frames werden pro Frame beim Einlesen übersprungen
    int mFrameCount;
    QImage mCurrentFrame;
    int mEffectiveFrameNumber;
    int mEffectiveFrameTime;
    int mDesiredFrameNumber;
    int mDesiredFrameTime;
    int mPreRenderFrameNumber;

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

    static QString ms2hmsz(int ms, bool withMs = true);
    static QString strippedName(const QString& fullFileName);
};

#endif // MAINWINDOW_H

