# $Id$
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>, Heise Zeitschriften Verlag. All rights reserved.

QT += core gui xml

CONFIG += warn_on thread qt help

TARGET = VStripe
TEMPLATE = app

TRANSLATIONS = VStripe_de.ts

CODECFORTR = UTF-8


win32 {
OPENCVDIR = C:/OpenCV-2.2.0-MinGW
FFMPEGDIR = ../ffmpeg-static
RC_FILE = VStripe.rc
LIBS += -L$${FFMPEGDIR}/lib -lavcodec -lavformat -lavutil -lswscale
# LIBS += -L$${OPENCVDIR}/lib -lopencv_core220.dll -lopencv_highgui220.dll
}

unix:!macx {
FFMPEGDIR = ../ffmpeg-static
LIBS += -L$${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
LIBS += -L$${OPENCVDIR}/lib -lopencv_core -lopencv_highgui
}

macx {
FFMPEGDIR = /opt/local
OPENCVDIR = /opt/local
ICON = VStripe.icns
QMAKE_INFO_PLIST = VStripe.plist
LIBS += -L$${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
LIBS += -L$${OPENCVDIR}/lib -lopencv_core -lopencv_highgui
}

INCLUDEPATH += $${FFMPEGDIR}/include \
    $${FFMPEGDIR}/include/libavcodec \
    $${FFMPEGDIR}/include/libavformat \
    $${FFMPEGDIR}/include/libavutil \
    $${FFMPEGDIR}/include/libavdevice \
    $${FFMPEGDIR}/include/libavfilter \
    $${FFMPEGDIR}/include/libswscale \
    $${OPENCVDIR}/include \
    $${OPENCVDIR}/include/opencv \
    $${OPENCVDIR}/include/opencv2


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
    helpbrowser.cpp
#    webcam.cpp

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
    helpbrowser.h \
    abstractvideodecoder.h
#    webcam.h

FORMS += mainwindow.ui \
    previewform.ui

RESOURCES += vstripe.qrc
