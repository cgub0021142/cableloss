#-------------------------------------------------
#
# Project created by QtCreator 2020-08-31T10:15:34
#
#-------------------------------------------------

QT       += core gui
QT +=network
QT += SVG

TARGET = cable_loss_0831
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    tablemodel.cpp \
    tabledelegate.cpp \
    standardtable.cpp

HEADERS  += widget.h \
    tablemodel.h \
    tabledelegate.h \
    standardtable.h

FORMS    += widget.ui

RESOURCES += \
    res.qrc
