#-------------------------------------------------
#
# Project created by QtCreator 2013-10-08T10:45:15
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DME
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.md \
    LICENSE

win32 {
    LIBS += C:\Qt\Tools\mingw48_32\i686-w64-mingw32\lib\libws2_32.a
}

macx {
    #LIBS += -lsock
}
