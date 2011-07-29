# $Id$
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>, Heise Zeitschriften Verlag. All rights reserved.

QT += core gui xml

CONFIG += warn_on thread qt

TARGET = VStripe
TEMPLATE = app

TRANSLATIONS = VStripe_de.ts

CODECFORTR = UTF-8

QMAKE_CXXFLAGS += -fopenmp

win32 {
OPENCVDIR = C:/OpenCV-2.2.0-MinGW
FFMPEGDIR = ../ffmpeg-static
RC_FILE = VStripe.rc
LIBS += -L$${FFMPEGDIR}/lib -lavcodec -lavformat -lavutil -lswscale
LIBS += -L$${OPENCVDIR}/lib -lopencv_core220.dll -lopencv_highgui220.dll
LIBS += -lgomp
}

unix:!macx {
OPENCVDIR = /usr
FFMPEGDIR = ../ffmpeg-static
LIBS += -L$${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
LIBS += -L$${OPENCVDIR}/lib -lcv -lhighgui
LIBS += -lgomp
}

macx {
FFMPEGDIR = /opt/local
OPENCVDIR = /opt/local
ICON = VStripe.icns
QMAKE_INFO_PLIST = VStripe.plist
LIBS += -L$${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
LIBS += -L$${OPENCVDIR}/lib -lopencv_core -lopencv_highgui
LIBS += -lgomp
}

INCLUDEPATH += $${FFMPEGDIR}/include \
    $${FFMPEGDIR}/include/libavcodec \
    $${FFMPEGDIR}/include/libavformat \
    $${FFMPEGDIR}/include/libavutil \
    $${FFMPEGDIR}/include/libavdevice \
    $${FFMPEGDIR}/include/libavfilter \
    $${FFMPEGDIR}/include/libswscale \
    $${OPENCVDIR}/include \
    $${OPENCVDIR}/include/opencv


DEFINES += __STDC_CONSTANT_MACROS

SOURCES += main.cpp\
    mainwindow.cpp \
    videowidget.cpp \
    videoreaderthread.cpp \
    picturewidget.cpp \
    videodecoder.cpp \
    markableslider.cpp \
    project.cpp \
    histogram.cpp \
    previewform.cpp \
    webcam.cpp \
    webcamthread.cpp \
    picturesizedialog.cpp \
    kineticscroller.cpp

HEADERS += mainwindow.h \
    videowidget.h \
    videoreaderthread.h \
    picturewidget.h \
    videodecoder.h \
    ffmpeg.h \
    markableslider.h \
    project.h \
    histogram.h \
    previewform.h \
    abstractvideodecoder.h \
    webcam.h \
    webcamthread.h \
    picturesizedialog.h \
    kineticscroller.h

FORMS += mainwindow.ui \
    previewform.ui \
    picturesizedialog.ui

RESOURCES += vstripe.qrc
