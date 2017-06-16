#-------------------------------------------------
#
# Project created by QtCreator 2017-06-13T21:41:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Nearest-point-on-mesh
TEMPLATE = app

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
        mainwindow.cpp \
    main3dscene.cpp \
    model.cpp \
    model_fileloader_off.cpp \
    model_primitiveloader.cpp \
    model_calculatable.cpp

HEADERS += \
        mainwindow.h \
    main3dscene.h \
    model.h \
    model_fileloader_off.h \
    model_primitiveloader.h \
    model_calculatable.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    backlog.txt
