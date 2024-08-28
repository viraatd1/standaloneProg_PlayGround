TEMPLATE = app
CONFIG += c++11 console
QT += network qml quick multimedia serialport charts core concurrent

QMAKE_LFLAGS += -v

#LIBS += -L/opt/reach/sdk/imx6dl-g3-sd/FSL-QT5-1.1.1/sysroots/armv7at2hf-neon-reach-linux-gnueabi/usr/lib/ -lftd2xx.so
#LIBS += -L/usr/lib/ -lftd2xx.so

LIBS += -lftd2xx -lpthread

SOURCES += \
        #main.c \
        newMain.c \
        serialWrapperFDTI.cpp

HEADERS += \
    WinTypes.h \
    ftd2xx.h \
    serialWrapperFDTI.h
