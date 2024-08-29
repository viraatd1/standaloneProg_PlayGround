#ifndef SERIAL_WRAPPER_FTDI_H
#define SERIAL_WRAPPER_FTDI_H

#include "ftd2xx.h"

/**
 * @brief Wrapper class for the FTDI device using the libftd2xx library.
 */
class serialWrapperFTDI {

public:
    /**
     * @brief Constructor for the serialWrapperFTDI class.
     */
    serialWrapperFTDI();

    /**
     * @brief Destructor for the serialWrapperFTDI class.
     */
    ~serialWrapperFTDI();

    /**
     * @brief Initializes the FTDI device.
     * @return 0 on success, -1 on failure.
     */
    int initialize();

    /**
     * @brief Reads data from the FTDI device.
     * @param arg_bytesToRead Number of bytes to read.
     * @param arg_bytesRead Reference to store the number of bytes actually read.
     * @param arg_buffer Pointer to the buffer where the read data will be stored.
     * @return 0 on success, -1 on failure.
     */
    int readData(unsigned int arg_bytesToRead, unsigned int& arg_bytesReturned, unsigned char* arg_buffer);

    /**
     * @brief Clears the FTDI device buffers.
     * @return 0 on success, -1 on failure.
     */
    int cleanBuffer();

private:
    FT_HANDLE m_ftHandle;           /* Handle for the FTDI device */
    FT_STATUS m_status;             /* API Calls return status validation */
    unsigned char m_buffer[2000]={0}; /* Buffer to read data from the FTDI device */
};

#endif // SERIAL_WRAPPER_FTDI_H
