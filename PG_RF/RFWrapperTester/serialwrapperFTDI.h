#ifndef SERIALWRAPPERFTDI_H
#define SERIALWRAPPERFTDI_H

#include "./external/lib_FTDISerial/include/ftd2xx.h"

#define FTDI_SUCCESS 0
#define FTDI_FAILURE -1
#define DELAY_IN_MICROSECONDS 100000 // 100 milliseconds

class serialWrapperFTDI
{
public:
    serialWrapperFTDI();
    ~serialWrapperFTDI();

    int initialize();
    int readData(unsigned char* arg_buffer, unsigned int arg_bytesToRead, unsigned int* arg_bytesReturned);
    int cleanBuffer();

private:
    FT_HANDLE m_ftHandle; /**< Handle for the FTDI device. */
};

#endif // SERIALWRAPPERFTDI_H
