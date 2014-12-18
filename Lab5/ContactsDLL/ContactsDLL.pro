#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T01:30:55
#
#-------------------------------------------------

QT       -= gui

TARGET = ContactsDLL
TEMPLATE = lib

DEFINES += CONTACTSDLL_LIBRARY

SOURCES += contactsdll.cpp \
    ThreadPool.cpp

HEADERS += contactsdll.h\
        contactsdll_global.h \
    ThreadPool.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
