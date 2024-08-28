/*
    Simple example to open a maximum of 4 devices - write some data then read it back.
    Shows one method of using list devices also.
    Assumes the devices have a loopback connector on them and they also have a serial number

    To build use the following gcc statement
    (assuming you have the d2xx library in the /usr/local/lib directory).
    gcc -o simple main.c -L. -lftd2xx -Wl,-rpath /usr/local/lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ftd2xx.h"

#define BUF_SIZE 0x7D0

#define MAX_DEVICES		1



static void dumpBuffer(unsigned char *buffer, int elements)
{
    int j;

    printf(" [");
    for (j = 0; j < elements; j++)
    {
        if (j > 0)
            printf(", ");
        printf("0x%02X", (unsigned int)buffer[j]);
    }
    printf("]\n");
}



int main()
{
    unsigned char 	cBufWrite[BUF_SIZE];
    unsigned char * pcBufRead = NULL;
    char * 	pcBufLD[MAX_DEVICES + 1];
    char 	cBufLD[MAX_DEVICES][64];
    DWORD	dwRxSize = 0;
    DWORD 	dwBytesWritten, dwBytesRead;


    FT_STATUS	ftStatus;                   // Device status
    FT_HANDLE	ftHandle[MAX_DEVICES];      // Device handle
    int	iNumDevs = 0;
    int	i, j;
    int	iDevicesOpen;

    for(i = 0; i < MAX_DEVICES; i++) {
        pcBufLD[i] = cBufLD[i];
    }
    pcBufLD[MAX_DEVICES] = NULL;

    ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

    if(ftStatus != FT_OK) {
        printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
        return 1;
    }

    for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ); i++) {
        printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
    }

    for(j = 0; j < BUF_SIZE; j++) {
        cBufWrite[j] = j;
    }

    for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ) ; i++) {
        /* Setup */
        if((ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[i])) != FT_OK){
            /*
                This can fail if the ftdi_sio driver is loaded
                use lsmod to check this and rmmod ftdi_sio to remove
                also rmmod usbserial
            */
            printf("Error FT_OpenEx(%d), device %d\n", (int)ftStatus, i);
            printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
            printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
            return 1;
        }

        printf("Opened device %s\n", cBufLD[i]);

        iDevicesOpen++;
        if((ftStatus = FT_SetBaudRate(ftHandle[i], 115200)) != FT_OK) {
            printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int)ftStatus, cBufLD[i]);
            break;
        }

        // Set data characteristics (8 data bits, no parity, 1 stop bit)
        ftStatus = FT_SetDataCharacteristics(ftHandle[i], FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
        if (ftStatus != FT_OK) {
            printf("Error setting data characteristics: %d\n", ftStatus);
            FT_Close(ftHandle[i]);
            return EXIT_FAILURE;
        }

        // Set flow control to none
        ftStatus = FT_SetFlowControl(ftHandle[i], FT_FLOW_NONE, 0, 0);
        if (ftStatus != FT_OK) {
            printf("Error setting flow control: %d\n", ftStatus);
            FT_Close(ftHandle[i]);
            return EXIT_FAILURE;
        }



        printf("Calling FT_Write with this write-buffer:\n");
//        dumpBuffer(cBufWrite, BUF_SIZE);

//        /* Write */
//        ftStatus = FT_Write(ftHandle[i], cBufWrite, BUF_SIZE, &dwBytesWritten);
//        if (ftStatus != FT_OK) {
//            printf("Error FT_Write(%d)\n", (int)ftStatus);
//            break;
//        }
//        if (dwBytesWritten != (DWORD)BUF_SIZE) {
//            printf("FT_Write only wrote %d (of %d) bytes\n",
//                   (int)dwBytesWritten,
//                   BUF_SIZE);
//            break;
//        }
//        sleep(1);

        /* Read */
        dwRxSize = 0;
        while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
            ftStatus = FT_GetQueueStatus(ftHandle[i], &dwRxSize);
            printf("current Size %d \n",dwRxSize);
        }
        if(ftStatus == FT_OK) {
            pcBufRead = realloc(pcBufRead, dwRxSize);
            memset(pcBufRead, 0xFF, dwRxSize);
            printf("Calling FT_Read with this read-buffer:\n");
            dumpBuffer(pcBufRead, dwRxSize);
            ftStatus = FT_Read(ftHandle[i], pcBufRead, dwRxSize, &dwBytesRead);
            if (ftStatus != FT_OK) {
                printf("Error FT_Read(%d)\n", (int)ftStatus);
                break;
            }
            if (dwBytesRead != dwRxSize) {
                printf("FT_Read only read %d (of %d) bytes\n",
                       (int)dwBytesRead,
                       (int)dwRxSize);
                break;
            }
            printf("FT_Read read %d bytes.  Read-buffer is now:\n",
                   (int)dwBytesRead);
            dumpBuffer(pcBufRead, (int)dwBytesRead);
            if (0 != memcmp(cBufWrite, pcBufRead, BUF_SIZE)) {
                printf("Error: read-buffer does not match write-buffer.\n");
                break;
            }
            printf("%s test passed.\n", cBufLD[i]);
        }
        else {
            printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);
        }

    }

    iDevicesOpen = i;
    /* Cleanup */
    for(i = 0; i < iDevicesOpen; i++) {
        FT_Close(ftHandle[i]);
        printf("Closed device %s\n", cBufLD[i]);
    }

    if(pcBufRead)
        free(pcBufRead);
    return 0;
}












































//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "ftd2xx.h"

//#define READ_BUFFER_SIZE 256
//#define LOG_DATA 970

//int main() {
//    printf("viraat--start");
//    FT_HANDLE ftHandle;
//    FT_STATUS ftStatus;
//    char readBuffer[READ_BUFFER_SIZE];
//    DWORD bytesRead;

//    printf("viraat--1");

//    // Initialize the FTDI device
//    ftStatus = FT_Open(0, &ftHandle); // Open the first available device
//    if (ftStatus != FT_OK) {
//        printf("Error opening FTDI device: %d\n", ftStatus);
//        return EXIT_FAILURE;
//    }else {
//        printf("viraat-0");
//    }

//    // Set the baud rate (115200 for example)
//    ftStatus = FT_SetBaudRate(ftHandle, 115200);
//    if (ftStatus != FT_OK) {
//        printf("Error setting baud rate: %d\n", ftStatus);
//        FT_Close(ftHandle);
//        return EXIT_FAILURE;
//    }

//    //m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
//    //m_serialPort->setParity(QSerialPort::Parity::NoParity);
//    //m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
//    //m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

//    // Set data characteristics (8 data bits, no parity, 1 stop bit)
//    ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
//    if (ftStatus != FT_OK) {
//        printf("Error setting data characteristics: %d\n", ftStatus);
//        FT_Close(ftHandle);
//        return EXIT_FAILURE;
//    }

//    // Set flow control to none
//    ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0);
//    if (ftStatus != FT_OK) {
//        printf("Error setting flow control: %d\n", ftStatus);
//        FT_Close(ftHandle);
//        return EXIT_FAILURE;
//    }

//    printf("viraat-1");

//    // Read data from the buffer
//    memset(readBuffer, 0, READ_BUFFER_SIZE);

//    printf("viraat-2");

//    unsigned int remote_bytes;
//    DWORD bytestransferred, rxbytes=0;
//    FT_GetQueueStatus(ftHandle,&rxbytes);

//    printf("viraat-3");
//    printf("FT_GetQueueStatus:  ", rxbytes);

//    printf("viraat-4");
//    if (rxbytes==(LOG_DATA*2))
//    {
//        printf("viraat-5");
//        remote_bytes=LOG_DATA*2;
//    }
////	else if (rxbytes==good_bytes)
////	{
////		remote_bytes=good_bytes;
////	}
////    else {
////        remote_bytes=0;
////    }

//    printf("viraat-6");
//    ftStatus = FT_Read(ftHandle, /*(uint8*)&*/readBuffer, 1940 - 1, &bytestransferred);
//    if (ftStatus != FT_OK) {
//        printf("Error reading from FTDI device: %d\n", ftStatus);
//        FT_Close(ftHandle);
//        return EXIT_FAILURE;
//    }

//    // Null-terminate the read buffer and print the data
//    readBuffer[bytesRead] = '\0';
//    printf("Data read from device: %s\n", readBuffer);
//    printf("viraat-7");


//    // Clean the buffers before reading
//    ftStatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
//    if (ftStatus != FT_OK) {
//        printf("Error purging buffers: %d\n", ftStatus);
//        FT_Close(ftHandle);
//        return EXIT_FAILURE;
//    }



//    // Close the FTDI device
//    FT_Close(ftHandle);

//    return EXIT_SUCCESS;
//}
