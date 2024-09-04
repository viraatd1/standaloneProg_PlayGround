#include "serialWrapperFDTI.h"
#include <iostream>
#include <unistd.h>

#define FTDI_SUCCESS 0
#define FTDI_FAILURE -1
#define DELAY_IN_MICROSECONDS 100000 // 100 milliseconds
#define MAX_BUFFER_SIZE 4096

DWORD baudRate = 622750; // Set the desired baud rate

serialWrapperFTDI::serialWrapperFTDI()
    : m_ftHandle(nullptr)
    , m_status(FT_OK)
{

}

serialWrapperFTDI::~serialWrapperFTDI() {
    if (m_ftHandle) {
        FT_Close(m_ftHandle);
    }
}

int serialWrapperFTDI::initialize() {

    /* Open the Device Handle */
    m_status = FT_Open(0, &m_ftHandle);
    if (m_status != FT_OK) {
        std::cout << "Failed to open FTDI device." << std::endl;
        return FTDI_FAILURE;  // Initialization failed
    }

    // Set the baud rate
    m_status = FT_SetBaudRate(m_ftHandle, baudRate);
    if (m_status != FT_OK) {
        std::cout << "Error setting baud rate: "<< baudRate <<", status: "<< m_status;
        FT_Close(m_ftHandle);
        return FTDI_FAILURE;
    }

    // Set data characteristics: 8 data bits, no parity, 1 stop bit
    m_status = FT_SetDataCharacteristics(m_ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
    if (m_status != FT_OK) {
        std::cout << "Error setting data characteristics:" << m_status;
        FT_Close(m_ftHandle);
        return FTDI_FAILURE;
    }

    // Set flow control: none
    m_status = FT_SetFlowControl(m_ftHandle, FT_FLOW_NONE, 0, 0);
    if (m_status != FT_OK) {
        std::cout << "Error setting flow control:" << m_status;
        FT_Close(m_ftHandle);
        return FTDI_FAILURE;
    }

    // Set timeout  read 1 sec write 100msec
    m_status = FT_SetTimeouts(m_ftHandle, 1000, 10); // 10 msec to read 10 msec to write
    if (m_status != FT_OK){
        std::cout << "Error setting timeouts : " << m_status;
        return FTDI_FAILURE;
    }

    // Clear the buffer if there are any garbage data
    m_status = FT_Purge(m_ftHandle, FT_PURGE_RX);
    if (m_status != FT_OK) {
        std::cout << "Error clearing receive buffer:" << m_status;
        return FTDI_FAILURE;
    }

    return FTDI_SUCCESS;  // Initialization successful
}



int serialWrapperFTDI::readData(unsigned int arg_bytesToRead, unsigned int& arg_bytesReturned, unsigned char* arg_buffer)
{
    DWORD lcl_bytesAvailable = 0;

    arg_bytesReturned = 0;

    /* This has to be a Loop and the exit points are designed such that fetching the data until
            1. the available bytes are equal to the number from the argument
            2. purge buffer signal is received from the controller
    */
    while (true) {

        /* Get the number of bytes available in the receive buffer */
        m_status = FT_GetQueueStatus(m_ftHandle, &lcl_bytesAvailable);
        if (m_status != FT_OK) {
            std::cout << "Error getting queue status: " << m_status;
            FT_Close(m_ftHandle);
            return 1; /* ToDo : Log a High Priority Message or re-initialize */
        }

        // Limit the bytesRead to MAX_BUFFER_SIZE
        //DWORD lcl_bytesToRead = (bytesAvailable > MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : bytesAvailable;


        FT_STATUS lcl_status = FT_Read(m_ftHandle, arg_buffer, arg_bytesToRead, &arg_bytesReturned);
        if (lcl_status != FT_OK) {
            std::cerr << "Failed to read data from FTDI device." << std::endl;
            return FTDI_FAILURE;  // Read failed
        }
        usleep(DELAY_IN_MICROSECONDS);

        //        if (arg_bytesToRead > lcl_bytesAvailable) {
        //            std::cerr << "Requested bytes to read exceed available bytes in the buffer." << std::endl;
        //            return FTDI_FAILURE;  // Error: Not enough bytes available to read
        //        }
    }

    return FTDI_SUCCESS;  // Read successful
}

int serialWrapperFTDI::cleanBuffer() {
    FT_STATUS lcl_status = FT_Purge(m_ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    if (lcl_status != FT_OK) {
        std::cerr << "Failed to purge FTDI device buffers." << std::endl;
        return FTDI_FAILURE;  // Purge failed
    }
    return FTDI_SUCCESS;  // Purge successful
}
