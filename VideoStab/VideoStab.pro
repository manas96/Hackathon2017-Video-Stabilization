#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T20:47:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

# add open CV
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

TARGET = VideoStab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    stabilizer.cpp \
    qcustomplot.cpp \
    displaystabilized.cpp


HEADERS  += mainwindow.h \
    player.h \
    stabilizer.h \
    qcustomplot.h \
    displaystabilized.h



FORMS    += mainwindow.ui
