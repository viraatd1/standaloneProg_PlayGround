/*
    Simple example to read a large amount of data from a BM device.
    Device must have bitbang capabilities to enable this to work

    To build use the following gcc statement
    (assuming you have the d2xx library in the /usr/local/lib directory).
    gcc -o largeread main.c -L. -lftd2xx -Wl,-rpath /usr/local/lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <vector>

#include "./external/lib_FTDISerial/include/ftd2xx.h"
#include "serialwrapperFTDI.h"

#define BUF_SIZE 0x100000		// 1 Megabyte

using namespace std;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

int main(int argc, char *argv[])
{
    // Use a smart pointer to manage the serialWrapperFTDI instance
    std::unique_ptr<serialWrapperFTDI> serialWrapperPtr = make_unique<serialWrapperFTDI>();

    // Initialize the FTDI device
    if (serialWrapperPtr->initialize() != 0) {
        return -1;  // Exit if initialization fails
    }

    // Example usage of readData
    std::vector<unsigned char> buffer;
    unsigned int lcl_bytesReturned;
    int bytesRead = serialWrapperPtr->readData( buffer.data(), 974*2, &lcl_bytesReturned);  // Read 100 bytes

    if(bytesRead == 0){
        std::cout << "Read " << bytesRead << " bytes from the device.\n";
    }else {
        std::cout << "Error reading data.\n" << bytesRead;
    }

    // Clean the buffers
    if (serialWrapperPtr->cleanBuffer() != 0) {
        std::cerr << "Error cleaning buffers.\n";
    }

    // No need to manually delete, unique_ptr will handle it
    return 0;  // Successful execution
}
