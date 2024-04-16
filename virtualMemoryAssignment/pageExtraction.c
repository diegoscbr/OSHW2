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

int pagetable[PAGES];
signed char main_Memo[MEMO_SIZE];
signed char *backing_ptr;

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("USAGE: <./a.out> <input file>\n");
        exit(0);
    }

    for (int i = 0; i < PAGES; i++) {
        pagetable[i] = -1;
    }

    const char *file_name = "BACKING_STORE.bin";
    const char *input_file = argv[1];
    const char *output_file = "output.txt";
    int backing_ptr_fd = open(file_name, O_RDONLY);
    backing_ptr = mmap(0, MEMO_SIZE, PROT_READ, MAP_PRIVATE, backing_ptr_fd, 0);
    FILE *input_fp = fopen(input_file, "r");
    FILE *output_fp = fopen(output_file, "w");
    char buf[BUF_SIZE];
    unsigned char freePage = 0;
    int total_addr = 0, pageFault = 0;

    while (fgets(buf, BUF_SIZE, input_fp) != NULL) {
        int logical_addr = atoi(buf);
        int offset = logical_addr & OFFSET_MASK;
        int logical = (logical_addr >> OFFSET_BITS);
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
    }

    printf("Number of Translated Addresses = %d\n", total_addr);
    printf("Page faults = %d\n", pageFault);
    printf("Page Fault Rate = %.1f %%\n", ((float)pageFault / total_addr) * 100);

    return 0;
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

