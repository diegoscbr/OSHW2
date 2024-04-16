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

int pagetable[PAGES];

void initializePageTable(int *pageTable);
int getPageNuber(int virtualAddress);
int getOffset(int virtualAddress);
long getFileSize(FILE *file);
signed char *mapFileIntoMemory(const char *file_name);


int main(int argc, const char *argv[]) {
    const char *input_file = argv[1];
    if (argc != 2) {printf("USAGE: <./a.out> <input file>\n");exit(0);}
    initializePageTable(pagetable);
    backing_ptr = mapFileIntoMemory(file_name);
    
    FILE *input_fp = fopen(input_file, "r");
    FILE *output_fp = fopen(output_file, "w");
    char buf[BUF_SIZE];
    unsigned char freePage = 0;
    int total_addr = 0, pageFault = 0;
//this while loop should be a function that takes in the file pointer and the page table
    while (fgets(buf, BUF_SIZE, input_fp) != NULL) {
        int logical_addr = atoi(buf);
        printf("Logical Address: %d\n", logical_addr);
        int offset = getOffset(logical_addr);
        int logical = getPageNuber(logical_addr);
        int physical = pagetable[logical];

        total_addr++;

        if (physical == -1) { // Page Fault
            pageFault++;

            physical = freePage;
            freePage++;

            memcpy(main_Memo + physical * PAGE_SIZE, backing_ptr + logical * PAGE_SIZE, PAGE_SIZE);

            pagetable[logical] = physical;
        }

        int physical_addr = (physical << OFFSET_BITS) | offset;
        signed char value = main_Memo[physical * PAGE_SIZE + offset];

        fprintf(output_fp, "Logical address: %d Physical address: %d Value: %d\n", logical_addr, physical_addr, value);
    } //end while loop

    printf("Number of Translated Addresses = %d\n", total_addr);
    printf("Page faults = %d\n", pageFault);
    printf("Page Fault Rate = %.1f %%\n", ((float)pageFault / total_addr) * 100);

    return 0;
}


void initializePageTable(int *pageTable) {
    for (int i = 0; i < PAGES; i++) {
        pageTable[i] = -1;
    }
}

long getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    clearerr(file);
    return file_size;
}

signed char *mapFileIntoMemory(const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    long fileSize = getFileSize(file);
    signed char *buffer = (signed char *)malloc(fileSize);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Read the file into memory
    size_t result = fread(buffer, 1, fileSize, file);
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

