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
    config.cpp \
    gameobjectview.cpp \
    hoverpoints.cpp \
    gameobjectcontainer.cpp \
    propertyeditordelegate.cpp

HEADERS  += mainwindow.h \
    editor.h \
    jsonio.h \
    util.h \
    property.h \
    propertymodel.h \
    gameobjectmodel.h \
    gameobject.h \
    config.h \
    gameobjectview.h \
    hoverpoints.h \
    gameobjectcontainer.h \
    propertyeditordelegate.h

FORMS    += mainwindow.ui \
    editor.ui \
    gameobjectcontainer.ui

RESOURCES += \
    icons.qrc
