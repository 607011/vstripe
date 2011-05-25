# $Id$
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>, Heise Zeitschriften Verlag. All rights reserved.

QT += core gui xml

TARGET = VStripe
TEMPLATE = app

win32 {
FFMPEGDIR = ../ffmpeg-static
RC_FILE = VStripe.rc
LIBS += -L$${FFMPEGDIR}/lib -lavcodec.dll -lavformat.dll -lavutil.dll -lswscale.dll
}

unix:!macx {
FFMPEGDIR = ../ffmpeg-static
LIBS += -L$${FFMPEGDIR}/lib -lavcodec -lavformat -lavutil -lswscale
}

macx {
FFMPEGDIR = /opt/local
QMAKE_INFO_PLIST = VStripe.plist
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3
LIBS += -L$${FFMPEGDIR}/lib -lavcodec -lavformat -lavutil -lswscale
}

INCLUDEPATH += $${FFMPEGDIR}/include \
    $${FFMPEGDIR}/include/libavcodec \
    $${FFMPEGDIR}/include/libavformat \
    $${FFMPEGDIR}/include/libavutil \
    $${FFMPEGDIR}/include/libavdevice \
    $${FFMPEGDIR}/include/libavfilter \
    $${FFMPEGDIR}/include/libswscale


DEFINES += __STDC_CONSTANT_MACROS

SOURCES += main.cpp\
    mainwindow.cpp \
    videowidget.cpp \
    videoreaderthread.cpp \
    picturewidget.cpp \
    videodecoder.cpp \
    markableslider.cpp \
    project.cpp

HEADERS  += mainwindow.h \
    videowidget.h \
    videoreaderthread.h \
    picturewidget.h \
    videodecoder.h \
    ffmpeg.h \
    markableslider.h \
    project.h

FORMS    += mainwindow.ui

RESOURCES += vstripe.qrc
