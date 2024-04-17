#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAGES 256
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMO_SIZE PAGES * PAGE_SIZE
#define BUF_SIZE 10

const char *file_name = "BACKING_STORE.bin";
const char *output_file = "output.txt";
signed char main_Memo[MEMO_SIZE];
signed char *backing_ptr;
char buf[BUF_SIZE];
int pagetable[PAGES];
int TLB[16][2];
void initializeTLB(int *TLB);
void initializePageTable(int *pageTable);
int getPageNuber(int virtualAddress);
int getOffset(int virtualAddress);
int getDirtyBit(int virtualAddress);
long getFileSize(FILE *file);
signed char *populateSecondaryMem(const char *file_name);
void outputMessage(int total_addr, int pageFault, int dirtyBitCount);

int main(int argc, const char *argv[]) {
    const char *input_file = argv[1];
    FILE *input_fp = fopen(input_file, "r"); //open address file
    FILE *backingSTORE_fp = fopen(file_name, "rb"); //open backing store file
    FILE *output_fp = fopen(output_file, "w");  
    if (argc != 2) {printf("USAGE: <./a.out> <input file>\n");exit(0);}
    initializePageTable(pagetable);
    backing_ptr = populateSecondaryMem(file_name);
    
    unsigned char freePage = 0;
    int total_addr = 0, pageFault = 0, dirtyBitCount = 0;
//this while loop should be a function that takes in the file pointer and the page table

 while (fgets(buf, BUF_SIZE, input_fp) != NULL) {
        int logical_addr = atoi(buf);
        int offset = getOffset(logical_addr);
        int logicalPageNo = getPageNuber(logical_addr); //page number
        int physicalFrameNo = pagetable[logicalPageNo]; //frame number at same index as page number
        int dirtyBit = getDirtyBit(logical_addr); 
        if(dirtyBit == 1) dirtyBitCount++;
        total_addr++;


        if (physicalFrameNo == -1) { // Page Fault
            pageFault++; //increase page fault count
            physicalFrameNo = freePage; 
            freePage++;
            fseek(backingSTORE_fp, logicalPageNo * PAGE_SIZE, SEEK_SET);
            fread(main_Memo + physicalFrameNo * PAGE_SIZE, sizeof(char), PAGE_SIZE, backingSTORE_fp);
            pagetable[logicalPageNo] = physicalFrameNo; //update page table     
        }
        
        int physical_addr = (physicalFrameNo << OFFSET_BITS) | offset;
        signed char value = main_Memo[physicalFrameNo * PAGE_SIZE + offset];
//this can be replaced by a printf 
        fprintf(output_fp, "Logical address: %d Physical address: %d Value: %d Dirty Bit: %d\n", logical_addr, physical_addr, value, dirtyBit);
    } //end while loop
    fclose(input_fp);
    fclose(backingSTORE_fp);
    fclose(output_fp);
    outputMessage(total_addr, pageFault, dirtyBitCount);
    return 0;
}




void initializePageTable(int *pageTable) {
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = -1;
    }
}

long getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END); //gets position at end of file
    long fileSize = ftell(file); //gets the current position in the stream (last index of file stream = file size)
    fseek(file, 0, SEEK_SET); //reset position indicator back to front of file
    clearerr(file);
    return fileSize;
}

//takes backing_store file and maps it into memory
signed char *populateSecondaryMem(const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    long fileSize = getFileSize(file);
    signed char *buffer = (signed char *)malloc(fileSize); //allocate memory for file to be read into
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    size_t result = fread(buffer, 1, fileSize, file); //read file into buffer
    if (result != fileSize) {
        perror("Failed to read file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return buffer;
}


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
void outputMessage(int total_addr, int pageFault, int dirtyBitCount) {
    printf("Number of Translated Addresses = %d\n", total_addr);
    printf("Page Fault Rate = %.1f %%\n", ((float)pageFault / total_addr) * 100);
    printf("Number of Dirty Pages = %d\n", dirtyBitCount);
}
int getDirtyBit(int virtualAddress) {
    return (virtualAddress >> 16) & 0x1;
}