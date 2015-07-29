#-------------------------------------------------
#
# Project created by QtCreator 2015-05-22T20:09:38
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TopologyController
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    RunningStatus.cpp \
    Switch.cpp \
    SwitchStatus.cpp \
    ModifySwitchDialog.cpp \
    DeleteSwitchThread.cpp \
    ModifySwitchThread.cpp \
    AddSwitchThread.cpp \
    RunSwitchThread.cpp \
    SwitchData.cpp \
    PaintWidget.cpp \
    SerialPortThread.cpp

HEADERS  += MainWindow.h \
    RunningStatus.h \
    Switch.h \
    SwitchStatus.h \
    SwitchData.h \
    ModifySwitchDialog.h \
    DeleteSwitchThread.h \
    ModifySwitchThread.h \
    AddSwitchThread.h \
    RunSwitchThread.h \
    PaintWidget.h \
    SerialPortThread.h

FORMS    += mainwindow.ui \
    modifyswitchdialog.ui
