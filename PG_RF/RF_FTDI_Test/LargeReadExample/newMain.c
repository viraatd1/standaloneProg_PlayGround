#include <stdio.h>
#include <stdlib.h>
#include <ftd2xx.h>
#include <unistd.h> // For sleep function (if needed for waiting)

#define MAX_BUFFER_SIZE 4096
#define TIMEOUT_SECONDS 20 // Timeout for waiting for data

#define LOG_DATA 970

// Function to convert FT_STATUS to string
const char* FT_GetStatusString(FT_STATUS status) {
    switch (status) {
    case FT_OK: return "FT_OK";
    case FT_INVALID_HANDLE: return "FT_INVALID_HANDLE";
    case FT_DEVICE_NOT_FOUND: return "FT_DEVICE_NOT_FOUND";
    case FT_DEVICE_NOT_OPENED: return "FT_DEVICE_NOT_OPENED";
    case FT_IO_ERROR: return "FT_IO_ERROR";
    case FT_INSUFFICIENT_RESOURCES: return "FT_INSUFFICIENT_RESOURCES";
    case FT_INVALID_PARAMETER: return "FT_INVALID_PARAMETER";
    case FT_INVALID_BAUD_RATE: return "FT_INVALID_BAUD_RATE";
    default: return "UNKNOWN_ERROR";
    }
}

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

unsigned short pulse_data[974]={0};

int main() {

    FT_HANDLE ftHandle;
    FT_STATUS status;
    DWORD bytesAvailable, bytesRead;
    unsigned char buffer[2000]={0};
    //DWORD baudRate = 115200; // Set the desired baud rate
    DWORD baudRate = 622750; // Set the desired baud rate

    time_t startTime, currentTime;

    // Open the FTDI device
    status = FT_Open(0, &ftHandle);
    if (status != FT_OK) {
        fprintf(stderr, "Error opening FTDI device: %d (%s)\n", status, FT_GetStatusString(status));
        return 1;
    }

    // Set the baud rate
    status = FT_SetBaudRate(ftHandle, baudRate);
    if (status != FT_OK) {
        fprintf(stderr, "Error setting baud rate: %d (%s)\n", status, FT_GetStatusString(status));
        FT_Close(ftHandle);
        return 1;
    }

    // Set data characteristics: 8 data bits, no parity, 1 stop bit
    status = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
    if (status != FT_OK) {
        fprintf(stderr, "Error setting data characteristics: %d (%s)\n", status, FT_GetStatusString(status));
        FT_Close(ftHandle);
        return 1;
    }

    // Set flow control: none
    status = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0);
    if (status != FT_OK) {
        fprintf(stderr, "Error setting flow control: %d (%s)\n", status, FT_GetStatusString(status));
        FT_Close(ftHandle);
        return 1;
    }

    // Set timeout  read 1 sec write 100msec
    status = FT_SetTimeouts(ftHandle, 1000, 10); // 10 msec to read 10 msec to write
    if (status != FT_OK)
    {
        fprintf(stderr, "", "RS-232 Settings Failed!", status);
    }

    /* ================================================================================================ */
    // Clear the buffer even before reading
    status = FT_Purge(ftHandle, FT_PURGE_RX);
    if (status != FT_OK) {
        fprintf(stderr, "Error clearing receive buffer: %d (%s)\n", status, FT_GetStatusString(status));
    }

    // Get start time for timeout
    startTime = time(NULL);

    //printf("Size of DWORD: %zu bytes\n", sizeof(DWORD)); //4 Bytes
    //printf("Size of char: %zu bytes\n", sizeof(char));   // Bytes
    //printf("Size of unsigned int: %zu bytes\n", sizeof(unsigned int));   // Bytes
    //printf("Size of unsigned short : %zu bytes\n", sizeof(unsigned short));   // Bytes

    while (1) {

        /* Get the number of bytes available in the receive buffer */
        status = FT_GetQueueStatus(ftHandle, &bytesAvailable);
        if (status != FT_OK) {
            fprintf(stderr, "Error getting queue status: %d (%s)\n", status, FT_GetStatusString(status));
            FT_Close(ftHandle);
            return 1;
        }

        if (bytesAvailable /*== LOG_DATA * 2*/) {
            // Print the number of bytes available
            printf("Bytes available in buffer: %lu\n", bytesAvailable);

            // Limit the bytesRead to MAX_BUFFER_SIZE
            DWORD bytesToRead = (bytesAvailable > MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : bytesAvailable;

            // Read the available bytes from the buffer
            status = FT_Read(ftHandle, buffer, bytesToRead, &bytesRead);
            if (status != FT_OK) {
                fprintf(stderr, "Error reading from FTDI device: %d (%s)\n", status, FT_GetStatusString(status));
                FT_Close(ftHandle);
                return 1;
            }

            // Print bytes read
            //printf("Bytes read: %lu\n", bytesRead);

            // Check if the number of bytes read is even

            // Process the data read from the buffer
            printf("Read %lu bytes from buffer:\n", bytesRead);

            // Process each pair of bytes
            for (int i = 0; i < 974; i++) {
                // Ensure we have at least two bytes to process
                int index = i*2;

                // Ensure there are enought bytes in the read buffer
                if (i + 1 < bytesRead) {
                    unsigned short msb = (unsigned short)(buffer[index]);        //MSB
                    unsigned short lsb = (unsigned short)(buffer[index + 1]);    //LSB

                    pulse_data[i]= (unsigned short)((lsb + msb));
                    printf("Bytes: MSB : 0x%02X / %hu | LSB : 0x%02X / %hu | Val: %hu \n", buffer[index],msb, buffer[index+1],lsb,pulse_data[i]);
//                    unsigned short val = ((unsigned short)msb << 8) | (unsigned short)lsb ;
                    //printf("DataPoint # %d: %hu \n", i, pulse_data[i]);


                    //unsigned short shifted_msb = (msb << 8);
                    //pulse_data[i]= (unsigned short)(unsigned char)(shifted_msb + lsb);
                    // Print both bytes with 0x prefix and the integer value in both hexadecimal and decimal
                    //printf("Bytes: 0x%02X 0x%02X | %hu %hu -> Integer: 0x%04X (%u)\n", buffer[index], buffer[index+1],lsb, msb, (unsigned short)(unsigned char)(shifted_msb + lsb));
                }
            }

            // Clear the buffer
            status = FT_Purge(ftHandle, FT_PURGE_RX);
            if (status != FT_OK) {
                fprintf(stderr, "Error clearing receive buffer: %d (%s)\n", status, FT_GetStatusString(status));
            }
        }

        // Check for timeout
        //        currentTime = time(NULL);
        //        if (difftime(currentTime, startTime) >= TIMEOUT_SECONDS) {
        //            printf("Timeout reached. Exiting.\n");
        //            break;
        //        }

        // Sleep for a short period to avoid busy-waiting
        usleep(100000); // Sleep for 100 milliseconds
    }

    // Close the FTDI device
    FT_Close(ftHandle);
    return 0;
}
