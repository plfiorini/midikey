#-------------------------------------------------
#
# Project created by QtCreator 2015-04-30T13:29:09
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = midikey
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    keyfilter.cpp

include(QMidi/QMidi.pri)

HEADERS += \
    keyfilter.h
