#-------------------------------------------------
#
# Project created by QtCreator 2016-01-23T23:27:22
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = license_plate
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

LIBS += `pkg-config opencv --cflags --libs`
