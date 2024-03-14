#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CHARS 80

int main(int argc, char* argv[]){
    char *line = malloc(MAX_CHARS); 
    size_t len = 0;
    ssize_t read;

    FILE *outfile = stdout; // default to stdout
    int opt;
    printf("Chid Process PID: %d\n", getpid());
    printf("Enter a line of text\n");
    /* 
       From GNU.org: int getopt (int argc, char *const *argv, const char *options)
       The getopt function gets the next option argument 
       from the argument list specified by the argv and argc 
       arguments. Normally these values come directly from 
       the arguments received by main.*/


    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                perror("Error opening output file");
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if ((read = getline(&line, &len, stdin)) != -1) {
        fprintf(outfile, "%s", line);
    }

    if (outfile != stdout) {
        fclose(outfile);
    }

    return 0;
}