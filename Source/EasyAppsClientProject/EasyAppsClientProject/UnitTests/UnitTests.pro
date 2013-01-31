#-------------------------------------------------
#
# Project created by QtCreator 2012-06-12T01:38:28
#
#-------------------------------------------------

QT       += core testlib

QT       -= gui

TARGET = UnitTests
CONFIG   += console
CONFIG   -= app_bundle

PRECOMPILED_HEADER = ../EasyAppsClient/Developer/macros.h

TEMPLATE = app


SOURCES += main.cpp \
    ../EasyAppsClient/Common/Helpers/platformflagshelper.cpp \
    platformflagshelpertest.cpp

HEADERS += \
    ../EasyAppsClient/Common/Helpers/platformflagshelper.h \
    platformflagshelpertest.h \
    ../EasyAppsClient/Developer/macros.h
