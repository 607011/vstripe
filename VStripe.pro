# $Id$
# Copyright (c) 2011 Oliver Lau <oliver@von-und-fuer-lau.de>, Heise Zeitschriften Verlag. All rights reserved.

QT += core gui xml

CONFIG += warn_on thread qt help

TARGET = VStripe
TEMPLATE = app

TRANSLATIONS = VStripe_de.ts

CODECFORTR = UTF-8

# QMAKE_CXXFLAGS += -msse -msse2 -msse3 -msse4.1 -msse4.2

win32 {
FFMPEGDIR = ../ffmpeg-static
RC_FILE = VStripe.rc
LIBS += -L$${FFMPEGDIR}/lib -lavcodec.dll -lavformat.dll -lavutil.dll -lswscale.dll
}

unix:!macx {
FFMPEGDIR = ../ffmpeg-static
LIBS += -L$${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
}

macx {
FFMPEGDIR = /opt/local
OPENCVDIR = /opt/local
ICON = VStripe.icns
QMAKE_INFO_PLIST = VStripe.plist
LIBS += -L$${FFMPEGDIR}/lib \
    -lavformat \
    -lavcodec \
    -lavutil \
    -lswscale
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
    project.cpp \
    histogram.cpp \
    previewform.cpp

HEADERS  += mainwindow.h \
    videowidget.h \
    videoreaderthread.h \
    picturewidget.h \
    videodecoder.h \
    ffmpeg.h \
    markableslider.h \
    project.h \
    histogram.h \
    previewform.h

FORMS    += mainwindow.ui \
    previewform.ui

RESOURCES += vstripe.qrc

OTHER_FILES += \
    help.html \
    INSTALL.txt \
    TODO.txt \
    doc.qhp \
    doc/index.html
