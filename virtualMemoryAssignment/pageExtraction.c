#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define FRAME_SIZE 256 //8 bits
#define NUM_FRAMES 256
#define PAGE_SIZE 256 //2^8 = 8 bits
#define PAGE_TABLE_ENTRIES 256
#define MEMORY_SIZE FRAME_SIZE * NUM_FRAMES

#define OFFSET_BITS 8 //8 bits
#define PAGE_NUMBER_BITS 8 //8 bits

void getPageNumberAndOffset(int virtualAddress, int *pageNumber, int *offset) {
    // Mask to extract the page number (VPN) - high-order 8 bits
    *pageNumber = (virtualAddress >> 8) & 0xFF;
    // Mask to extract the offset - low-order 8 bits
    *offset = virtualAddress & 0xFF;
}
int testAddress[] = {1, 256, 32768, 32769, 128, 65534, 33153};
int main() {
    // Iterate over integers from 0 to 65535 (2^16 - 1)
    for (int i = 0; i <= 6; i++) {
        int pageNumber, offset;
        getPageNumberAndOffset(testAddress[i], &pageNumber, &offset);
        printf("Virtual Address: %5d, Page Number: %3d, Offset: %3d\n", testAddress[i], pageNumber, offset);
    }

    return 0;
}
