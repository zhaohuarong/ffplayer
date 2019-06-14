QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ffplayer
TEMPLATE = app

DESTDIR = $$PWD/bin

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -L$$PWD/lib -lVLCQtCore -lVLCQtWidgets
INCLUDEPATH += $$PWD/include

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui
