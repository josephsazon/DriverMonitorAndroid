#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T20:24:59
#
#-------------------------------------------------

QT       += core gui positioning location bluetooth widgets multimedia printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DriverMonitorAndroid
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
        drivermonitorandroid.cpp \
    chat.cpp \
    chatclient.cpp \
    chatserver.cpp \
    remoteselector.cpp \
    qcustomplot.cpp

HEADERS += \
        drivermonitorandroid.h \
    chat.h \
    chatclient.h \
    chatserver.h \
    remoteselector.h \
    qcustomplot.h

FORMS += \
        drivermonitorandroid.ui \
    chat.ui \
    remoteselector.ui

CONFIG += mobility
MOBILITY = location

RESOURCES += \
    resource.qrc

DISTFILES += \
    android-sources/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR =$$PWD/android-sources

