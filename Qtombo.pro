# -------------------------------------------------
# Project created by QtCreator 2010-03-06T13:42:53
# -------------------------------------------------
QT += core gui
QT += widgets

CONFIG += c++11

TARGET = Qtombo
TEMPLATE = app
INCLUDEPATH += .
INCLUDEPATH += src/
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/textinput.cpp \
    src/crypt/CryptManager.cpp \
    src/crypt/Crypt.cpp \
    src/GNUPG/md5.c \
    src/GNUPG/blowfish.c \
    src/qtombotreeview.cpp
HEADERS += src/mainwindow.h \
    src/textinput.h \
    src/crypt/CryptManager.h \
    src/crypt/Crypt.h \
    src/GNUPG/bithelp.h \
    src/qtombotreeview.h
FORMS += src/mainwindow.ui \
    src/textinput.ui
OTHER_FILES += 
RESOURCES += res/qtombo.qrc

QMAKE_CXXFLAGS += -fPIE

