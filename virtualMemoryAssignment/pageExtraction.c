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

int* pagetable[PAGE_TABLE_ENTRIES];
int tlb[TLB_SIZE];

int getPageNuber(int virtualAddress) {
    return (virtualAddress >> 8) & 0xFF;
}
int getOffset(int virtualAddress) {
    return virtualAddress & 0xFF;
}
int *setPageNumberAndOffset(int virtualAddress) {
    int *pageAndOffset = (int *)malloc(2 * sizeof(int));
    pageAndOffset[0] = getPageNuber(virtualAddress);
    pageAndOffset[1] = getOffset(virtualAddress);
    return pageAndOffset;
}
int testAddress[] = {1, 256, 512, 32769, 128, 65534, 33153};
int main() {

    int* dataStore[7];
    for (int i = 0; i <= 6; i++) {
        dataStore[i] = setPageNumberAndOffset(testAddress[i]);
        printf("Virtual Address: %5d, Page Number: %3d, Offset: %3d\n", testAddress[i], dataStore[i][0], dataStore[i][1]);
    }

    return 0;
}
