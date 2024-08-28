TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        serialwrapperFTDI.cpp \
        #/external/lib_FTDISerial/include/

LIBS += -lftd2xx -lpthread

HEADERS += \
    serialwrapperFTDI.h
