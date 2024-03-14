#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *file;
    int lineNumber = 0; // Line number
    srand(time(NULL));// Seed random number generator
    // Open the file
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return 1;
    }
        // Declare a pointer to a char and a size_t for getline
    char *line = NULL;
    /* 
    From Ubuntu Manpage:
        size_t Used  for  a  count of bytes.  It is the result of the sizeof() operator.  It is an
              unsigned integer type capable of storing values in the range [0, SIZE_MAX].

       ssize_t
              Used for a count of bytes or an error indication.  It  is  a  signed  integer  type
              capable of storing values al least in the range [-1, SSIZE_MAX].*/
    size_t len = 0; 
    ssize_t read;

    // Read and print each line
    while ((read = getline(&line, &len, file)) != -1) {
        lineNumber++;

        int waitTime = rand() % 4; // Wait for up to 4 seconds
        sleep(waitTime);
        printf("Line number: %d, Characters: %ld, Process ID: %d\n", lineNumber, read - 1, getpid());
    }

    // Free the line pointer
    free(line);

    // Close the file
    fclose(file);
    return 0;
}