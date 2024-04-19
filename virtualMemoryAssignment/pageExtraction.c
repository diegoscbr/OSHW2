#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "tlbQueue.h"

#define PAGES 256
#define PAGE_SIZE 256
#define MEMORY PAGES * PAGE_SIZE

#define OFFSET_BITS 8

#define BUF_SIZE 10
#define TLB_SIZE 16

#define EMPTY -1
const char *diskFile = "BACKING_STORE.bin";
const char *output_file = "output.txt";
signed char ramMem[MEMORY];
signed char *storagePointer;
char buf[BUF_SIZE];
int pagetable[PAGES];
int TLB[TLB_SIZE][2];
int dirtyPageMap[PAGES][2];
void initializeTLB(int tlb[][2], int rows);
void initializePageTable(int *pageTable);
int getPageNuber(int virtualAddress);
int getOffset(int virtualAddress);
int getPhysicalAddress(int frameNumber, int offset);
int indexIntoMemory(int frameNumber, int offset);   
int getDirtyBit(int virtualAddress);
long getFileSize(FILE *file);
signed char *populateSecondaryMem(const char *file_name);
void outputMessage(int pageFault, int dirtyBitCount, int tlbHit, int totalAddr);
void processLogicalAddress(FILE *input_fp, FILE *backingSTORE_fp, FILE *output_fp, int *pagetable,  int TLB[TLB_SIZE][2], char *ramMem, int dirtyPageMap[PAGES][2]);


int main(int argc, const char *argv[]) {
    const char *input_file = argv[1];
    FILE *input_fp = fopen(input_file, "r"); 
    FILE *backingSTORE_fp = fopen(diskFile, "rb"); 
    FILE *output_fp = fopen(output_file, "w");  
    if (argc != 2) {printf("USAGE: <./a.out> <input file>\n");exit(0);}
    initializePageTable(pagetable);
    initializeTLB(TLB, TLB_SIZE);
    storagePointer = populateSecondaryMem(diskFile);
    if (storagePointer == NULL) {printf("Failed to populate secondary memory\n");exit(1);}
   processLogicalAddress(input_fp, backingSTORE_fp, output_fp, pagetable, TLB, (char*)ramMem, dirtyPageMap);
    return 0;
}


void initializePageTable(int *pageTable) {
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = EMPTY;
    }
}
void initializeTLB(int tlb[][2], int rows) {
    for (int i = 0; i < rows; i++) {
        tlb[i][0] = EMPTY;
        tlb[i][1] = EMPTY;
    }
}
long getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END); 
    long fileSize = ftell(file); // (last index of file stream = file size)
    fseek(file, 0, SEEK_SET); //reset position 
    clearerr(file);
    return fileSize;
}

signed char *populateSecondaryMem(const char *fileName) {
    FILE *backingFile = fopen(fileName, "rb");
    if (backingFile == NULL) {
        perror("Unable to open file");
        exit(1);
    }
    long unsigned fileSize = getFileSize(backingFile);
    signed char *buffer = (signed char *)malloc(fileSize); //allocate memory for file to be read into
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    size_t result = fread(buffer, 1, fileSize, backingFile); //read file into buffer
    if (result != fileSize) {
        perror("Failed to read file");
        exit(1);
    }
    fclose(backingFile);
    return buffer;
}


int getPageNuber(int virtualAddress) {
    return (virtualAddress >> 8) & 0xFF;
}
int getOffset(int virtualAddress) {
    return virtualAddress & 0xFF;
}

int getPhysicalAddress(int frameNumber, int offset) {
    return (frameNumber << 8) | offset;
}
int indexIntoMemory(int frameNumber, int offset) {
    return frameNumber * PAGE_SIZE + offset;
}
int *setPageNumberAndOffset(int virtualAddress) {
    int *pageAndOffset = (int *)malloc(2 * sizeof(int));
    pageAndOffset[0] = getPageNuber(virtualAddress);
    pageAndOffset[1] = getOffset(virtualAddress);
    return pageAndOffset;
}
void outputMessage(int pageFault, int dirtyBitCount, int tlbHit, int totalAddr) {
    printf("Page Fault Rate = %.1f %%\n", ((float)pageFault / totalAddr) * 100);
    printf("Number of Dirty Pages = %d\n", dirtyBitCount);
    printf("TLB Hit Rate = %.1f %%\n", ((float)tlbHit / totalAddr) * 100);
}
int getDirtyBit(int virtualAddress) {
    return (virtualAddress >> 16) & 1;
}

void processLogicalAddress(FILE *input_fp, FILE *backingSTORE_fp, FILE *output_fp, int *pagetable, int TLB[TLB_SIZE][2], char *ramMem, int dirtyPageMap[PAGES][2]) {
    char buf[BUF_SIZE];
    unsigned char freePage = 0;
    int total_addr = 0, pageFault = 0, dirtyBitCount = 0, tlbHits = 0, tlbMisses = 0;

    while (fgets(buf, BUF_SIZE, input_fp) != NULL) {
        int logical_addr = atoi(buf); // Convert the line read into an integer
        int offset = getOffset(logical_addr);
        int logicalPageNo = getPageNuber(logical_addr); 

        // First, try to get the physical frame number from TLB
        int physicalFrameNo = EMPTY;
        for (int i = 0; i < TLB_SIZE; i++) {
            if (TLB[i][0] == logicalPageNo) {
                physicalFrameNo = TLB[i][1];
                tlbHits++;
                break;
            }
        }

        if (physicalFrameNo == EMPTY) {
            physicalFrameNo = pagetable[logicalPageNo];
            int replaceIndex = tlbMisses % TLB_SIZE; // Calculate index to replace
            TLB[replaceIndex][0] = logicalPageNo; // Update TLB 
            TLB[replaceIndex][1] = physicalFrameNo;
            tlbMisses++;
        }

        if (physicalFrameNo == -1) { // Page Fault if frame number still empty
            pageFault++; 
            physicalFrameNo = freePage;
            freePage++;
            fseek(backingSTORE_fp, logicalPageNo * PAGE_SIZE, SEEK_SET);
            fread(ramMem + (physicalFrameNo * PAGE_SIZE), sizeof(char), PAGE_SIZE, backingSTORE_fp);
            pagetable[logicalPageNo] = physicalFrameNo; // Update page table
        }

        int dirtyBit = getDirtyBit(logical_addr);
        if (dirtyBit == 1){
            dirtyBitCount++;
            dirtyPageMap[logicalPageNo][0] = logicalPageNo;
            dirtyPageMap[logicalPageNo][1] = 1;
        } 
        total_addr++;
        int physicalAddress = getPhysicalAddress(physicalFrameNo, offset);
        signed char value = ramMem[indexIntoMemory(physicalFrameNo, offset)];

        fprintf(output_fp, "Logical address: %08x Physical address: %08x Value: %08x Dirty Bit: %d TLB Hit: %d\n", logical_addr, physicalAddress, value, dirtyBit, tlbHits);
    }
    printf("TLB QUEUE: p\n");
    for (int i = 0; i < TLB_SIZE; i++) {
        printf("TLB[%d][0] = %d\n", i, TLB[i][0]);
        printf("TLB[%d][1] = %08x\n", i, TLB[i][1]);
    }
     printf("Dirty Page Map: p\n");
    for (int i = 0; i < PAGES; i++) {
        printf("DirtyPageMap[%d][0] = %d\n", i, dirtyPageMap[i][0]);
        printf("DirtyPageMap[%d][1] = %d\n", i, dirtyPageMap[i][1]);
    }


    fclose(input_fp);
    fclose(backingSTORE_fp);
    fclose(output_fp);
    outputMessage(pageFault, dirtyBitCount, tlbHits, total_addr);
}