QT       += core gui xml

TARGET = VStripe
TEMPLATE = app

FFMPEGDIR = ../ffmpeg-static

INCLUDEPATH += $${FFMPEGDIR}/include \
    $${FFMPEGDIR}/include/libavcodec \
    $${FFMPEGDIR}/include/libavformat \
    $${FFMPEGDIR}/include/libavutil \
    $${FFMPEGDIR}/include/libavdevice \
    $${FFMPEGDIR}/include/libavfilter \
    $${FFMPEGDIR}/include/libswscale

QMAKE_INFO_PLIST = VStripe.plist

DEFINES += __STDC_CONSTANT_MACROS

win32:LIBS += -L$${FFMPEGDIR}/lib -lavcodec.dll -lavformat.dll -lavutil.dll -lswscale.dll
unix:LIBS += -L$${FFMPEGDIR}/lib -lavcodec -lavformat -lavutil -lswscale

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

# RESOURCES += vstripe.qrc
