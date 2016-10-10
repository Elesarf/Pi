QT += core
QT += gui widgets

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/local/qt5pi/include/
INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/local/include/RF24/
INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/include/

LIBS += /home/elesar/opt/gcc-4.7-linaro-rpi-gnueabihf/arm-linux-gnueabihf/lib/libstdc++.so.6.0.20 #if soft be have bugs, then build GCC v4.9!!!!!!!

LIBS += -lrf24 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect

target.path = /sbin/
INSTALLS += target
TARGET = Protocol_b

TEMPLATE = app

SOURCES += main.cpp \
    protocol.cpp \
    jpgcrop.cpp \
    finder.cpp

HEADERS += \
    protocol.h \
    jpgcrop.h \
    finder.h






# SYSROOT / default ld-path
# default: /usr/lib, /lib, /etc/ld.so.conf.d/*.conf
