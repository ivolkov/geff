#-------------------------------------------------
#
# Project created by QtCreator 2014-11-07T13:12:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gproc-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    plots.cpp \
    ipc.cpp \
    eq.cpp \
    comp.cpp

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    ipc.h \
    plots.h \
    eq.h \
    comp.h

FORMS    += mainwindow.ui \
    eqdialog.ui \
    compdialog.ui
