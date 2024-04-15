#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
int get_offset(int address, int pageSize){
    return address & (pageSize - 1);
}
int main(int argc, char * argv){


    return 0;
}