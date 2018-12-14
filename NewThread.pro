#-------------------------------------------------
#
# Project created by QtCreator 2017-11-27T16:55:53
#
#-------------------------------------------------

QT += core gui
QT += serialport
INCLUDEPATH += D:/masterstu/C++/opencv/build/include/opencv \
D:/masterstu/C++/opencv/build/include/opencv2 \
D:/masterstu/C++/opencv/build/include


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += D:\masterstu\C++\opencv\build\x64\vc14\lib\opencv_world341.lib
LIBS += D:\masterstu\C++\opencv\build\x64\vc14\lib\opencv_world341d.lib

TARGET = NewThread
TEMPLATE = app

SOURCES += main.cpp\
    mythread.cpp \
    login.cpp \
    dialog.cpp \
    cameracalibration.cpp\
    lib/json/json_writer.cpp \
    lib/json/json_valueiterator.inl \
    lib/json/json_value.cpp \
    lib/json/json_reader.cpp \
    findcontour.cpp \
    pointscan.cpp

HEADERS  += \
    cameracalibration.h \
    dialog.h \
    login.h \
    mythread.h \
    lib/json/writer.h \
    lib/json/version.h \
    lib/json/value.h \
    lib/json/reader.h \
    lib/json/json.h \
    lib/json/json_tool.h \
    lib/json/forwards.h \
    lib/json/features.h \
    lib/json/config.h \
    lib/json/autolink.h \
    lib/json/assertions.h \
    findcontour.h \
    pointscan.h

FORMS    += dialog.ui \
    login.ui

RESOURCES += \
    icon.qrc
RC_FILE += icon.rc

DISTFILES += \
    mypaper/mypaper.doc

