#-------------------------------------------------
#
# Project created by QtCreator 2017-07-09T20:32:14
#
#-------------------------------------------------

QT       += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CStreamingCapture
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        linechart.cpp

HEADERS += \
        mainwindow.h \
        linechart.h \
    global_defines.h

FORMS += \
        mainwindow.ui

CONFIG    += qwt
CONFIG += console

win32: LIBS += -L$$quote(C:/Program Files/SP Devices/ADQAPI/) -lADQAPI
INCLUDEPATH += $$quote(C:/Program Files/SP Devices/ADQAPI/)
