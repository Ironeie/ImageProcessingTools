#-------------------------------------------------
#
# Project created by QtCreator 2018-08-06T17:13:00
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = imgproc
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        imgproc.cpp \
    combo.cpp \
    hist.cpp \
    fft.cpp \
    color.cpp

HEADERS += \
        imgproc.h \
    combo.h \
    hist.h \
    fft.h \
    color.h

FORMS += \
        imgproc.ui \
    combo.ui \
    hist.ui \
    fft.ui \
    color.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += D:\opencv\3.4.1\build\include

CONFIG(debug, debug|release): {
LIBS += -LD:\opencv\3.4.1\build\x64\vc15\lib \
-lopencv_world341d
} else:CONFIG(release, debug|release): {
LIBS += -LD:\opencv\3.4.1\build\x64\vc15\lib \
-lopencv_world341
}
