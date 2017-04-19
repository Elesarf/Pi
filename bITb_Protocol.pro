QT += core
QT -= gui widgets
QT += network

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += /mnt/raspberry_root_fs/usr/local/include/RF24/
INCLUDEPATH += /mnt/raspberry_root_fs/usr/local/lib/
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lrf24
#LIBS += /home/elesar/opt/gcc-4.7-linaro-rpi-gnueabihf/arm-linux-gnueabihf/lib/libstdc++.so.6.0.20 #if soft be have bugs, then build GCC v4.9!!!!!!!

target.path = /usr/bin
INSTALLS += target
TARGET = protocol_b

TEMPLATE = app

SOURCES += main.cpp \
    protocol.cpp \
    jpgcrop.cpp \
    finder.cpp \
    sender.cpp \
    long_hash.cpp \
    my_tcpsocket.cpp

HEADERS += \
    protocol.h \
    jpgcrop.h \
    finder.h \
    sender.h \
    long_hash.h \
    my_tcpsocket.h






# SYSROOT / default ld-path
# default: /usr/lib, /lib, /etc/ld.so.conf.d/*.conf
