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
    propertyeditordelegate.cpp \
    objecttype.cpp \
    objecttypemodel.cpp \
    expressiondesigner.cpp \
    expressionmodel.cpp \
    expression.cpp \
    expressioneditordelegate.cpp \
    newdocumentwizard.cpp

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
    propertyeditordelegate.h \
    objecttype.h \
    objecttypemodel.h \
    expressiondesigner.h \
    expressionmodel.h \
    expression.h \
    expressioneditordelegate.h \
    newdocumentwizard.h

FORMS    += mainwindow.ui \
    editor.ui \
    gameobjectcontainer.ui \
    expressiondesigner.ui \
    newdocumentwizard.ui

RESOURCES += \
    icons.qrc
