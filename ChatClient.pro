#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T16:16:36
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatClient
TEMPLATE = app


SOURCES += main.cpp\
        chatclient.cpp \
    inputdialog.cpp

HEADERS  += chatclient.h \
    inputdialog.h
