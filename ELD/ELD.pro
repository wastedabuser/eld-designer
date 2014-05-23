#-------------------------------------------------
#
# Project created by QtCreator 2014-05-20T19:02:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ELD
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    editor.cpp \
    jsonio.cpp \
    util.cpp \
    property.cpp \
    propertymodel.cpp \
    gameobjectmodel.cpp \
    gameobject.cpp \
    config.cpp

HEADERS  += mainwindow.h \
    editor.h \
    jsonio.h \
    util.h \
    property.h \
    propertymodel.h \
    gameobjectmodel.h \
    gameobject.h \
    config.h

FORMS    += mainwindow.ui \
    editor.ui

RESOURCES += \
    icons.qrc
