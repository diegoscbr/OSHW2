#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *file;
    char line[256];
    int lineNumber = 0;
    srand(time(NULL));

    // Open the file
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return 1;
    }

    // Read and print each line
    while (fgets(line, sizeof(line), file)) {
        lineNumber++;
        //printf("%s", line);
            int waitTime = rand() % 4; // Wait for up to 4 seconds
            sleep(waitTime);
            printf("Line number: %d, Characters: %ld, Process ID: %d\n", lineNumber, strlen(line) - 1, getpid());

    }

    // Close the file
    fclose(file);

    return 0;
}