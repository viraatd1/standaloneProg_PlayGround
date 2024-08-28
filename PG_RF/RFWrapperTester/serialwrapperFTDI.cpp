#include "serialwrapperFTDI.h"
#include <iostream>
#include <unistd.h>



serialWrapperFTDI::serialWrapperFTDI(): m_ftHandle(nullptr)
{

}

serialWrapperFTDI::~serialWrapperFTDI()
{
    if (m_ftHandle) {
        FT_Close(m_ftHandle);
    }
}

int serialWrapperFTDI::initialize()
{
    FT_STATUS lcl_status = FT_Open(0, &m_ftHandle);
    if (lcl_status != FT_OK) {
        std::cerr << "Failed to open FTDI device." << std::endl;
        return FTDI_FAILURE;  // Initialization failed
    }
    return FTDI_SUCCESS;  // Initialization successful
}

int serialWrapperFTDI::readData(unsigned char *arg_buffer, unsigned int arg_bytesToRead, unsigned int *arg_bytesReturned)
{
    DWORD lcl_bytesAvailable = 0;
    arg_bytesReturned = 0;

    while (true) {
        FT_GetQueueStatus(m_ftHandle, &lcl_bytesAvailable);
        if (lcl_bytesAvailable >= arg_bytesToRead) {
            break;
        }
        usleep(DELAY_IN_MICROSECONDS);
    }

    if (arg_bytesToRead > lcl_bytesAvailable) {
        std::cerr << "Requested bytes to read exceed available bytes in the buffer." << std::endl;
        return FTDI_FAILURE;  // Error: Not enough bytes available to read
    }

    FT_STATUS lcl_status = FT_Read(m_ftHandle, arg_buffer, arg_bytesToRead, arg_bytesReturned);
    if (lcl_status != FT_OK) {
        std::cerr << "Failed to read data from FTDI device." << std::endl;
        return FTDI_FAILURE;  // Read failed
    }
    return FTDI_SUCCESS;  // Read successful
}

int serialWrapperFTDI::cleanBuffer()
{
    FT_STATUS lcl_status = FT_Purge(m_ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    if (lcl_status != FT_OK) {
        std::cerr << "Failed to purge FTDI device buffers." << std::endl;
        return FTDI_FAILURE;  // Purge failed
    }
    return FTDI_SUCCESS;  // Purge successful
}
