#-------------------------------------------------
#
# Project created by QtCreator 2017-07-20T21:40:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicGetFirst
TEMPLATE = app

LIBS += -L$$PWD/zlib/lib -lzlibstatic

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h \
    zlib/include/crc32.h \
    zlib/include/deflate.h \
    zlib/include/gzguts.h \
    zlib/include/inffast.h \
    zlib/include/inffixed.h \
    zlib/include/inflate.h \
    zlib/include/inftrees.h \
    zlib/include/trees.h \
    zlib/include/zconf.h \
    zlib/include/zlib.h \
    zlib/include/zutil.h



FORMS += \
        mainwindow.ui
