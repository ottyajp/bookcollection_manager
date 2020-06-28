#-------------------------------------------------
#
# Project created by QtCreator 2017-09-26T21:36:44
#
#-------------------------------------------------

QT       += core gui webenginewidgets network serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = isbn
TEMPLATE = app

TRANSLATIONS = ja.ts

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        config.cpp \
        coverviewer.cpp \
        Db.cpp \
        Entity.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        config.h \
        coverviewer.h \
        Db.h \
        Entity.h \
        mainwindow.h

FORMS += \
        config.ui \
        coverviewer.ui \
        mainwindow.ui

RESOURCES += \
    resource.qrc
