#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T00:33:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-ContactsDLL-Desktop_Qt_5_4_0_MinGW_32bit-Debug/release/ -lContactsDLL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-ContactsDLL-Desktop_Qt_5_4_0_MinGW_32bit-Debug/debug/ -lContactsDLL

INCLUDEPATH += $$PWD/../ContactsDLL
DEPENDPATH += $$PWD/../ContactsDLL
