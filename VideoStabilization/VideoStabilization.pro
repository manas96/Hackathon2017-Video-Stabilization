#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T22:49:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

unix{
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

TARGET = VideoStabilization
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    qcustomplot.cpp \
    PracticalSocket.cpp \
    graphhandler.cpp

HEADERS  += mainwindow.h \
    player.h \
    qcustomplot.h \
    PracticalSocket.h \
    graphhandler.h

FORMS    += mainwindow.ui
