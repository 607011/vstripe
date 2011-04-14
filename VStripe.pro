QT       += core gui

TARGET = VStripe
TEMPLATE = app

INCLUDEPATH += C:/Workspace/ffmpeg-32-bit-static/include \
    C:/Workspace/ffmpeg-32-bit-static/include/libavcodec \
    C:/Workspace/ffmpeg-32-bit-static/include/libavformat \
    C:/Workspace/ffmpeg-32-bit-static/include/libavutil \
    C:/Workspace/ffmpeg-32-bit-static/include/libavdevice \
    C:/Workspace/ffmpeg-32-bit-static/include/libavfilter \
    C:/Workspace/ffmpeg-32-bit-static/include/libswscale

DEFINES += __STDC_CONSTANT_MACROS

LIBS += -LC:/Workspace/ffmpeg-32-bit-static/lib -lavcodec.dll -lavformat.dll -lavutil.dll -lswscale.dll

SOURCES += main.cpp\
    mainwindow.cpp \
    videowidget.cpp \
    videoreaderthread.cpp \
    picturewidget.cpp \
    videodecoder.cpp \
    markableslider.cpp

HEADERS  += mainwindow.h \
    videowidget.h \
    videoreaderthread.h \
    picturewidget.h \
    videodecoder.h \
    ffmpeg.h \
    markableslider.h

FORMS    += mainwindow.ui
