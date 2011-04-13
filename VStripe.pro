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
    framereader.cpp \
    videowidget.cpp \
    videoreaderthread.cpp

HEADERS  += mainwindow.h \
    framereader.h \
    videowidget.h \
    videoreaderthread.h

FORMS    += mainwindow.ui
