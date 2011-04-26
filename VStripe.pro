QT       += core gui xml

TARGET = VStripe
TEMPLATE = app

INCLUDEPATH += ../ffmpeg-32-bit-static/include \
    ../ffmpeg-32-bit-static/include/libavcodec \
    ../ffmpeg-32-bit-static/include/libavformat \
    ../ffmpeg-32-bit-static/include/libavutil \
    ../ffmpeg-32-bit-static/include/libavdevice \
    ../ffmpeg-32-bit-static/include/libavfilter \
    ../ffmpeg-32-bit-static/include/libswscale

DEFINES += __STDC_CONSTANT_MACROS

win32:LIBS += -L../ffmpeg-32-bit-static/lib -lavcodec.dll -lavformat.dll -lavutil.dll -lswscale.dll
unix:LIBS += -L../ffmpeg-32-bit-static/lib -lavcodec -lavformat -lavutil -lswscale

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

RESOURCES += \
    vstripe.qrc
