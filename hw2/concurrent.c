#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
int main(int argc, char *argv[]) {

    printf("This is the Parent Process\n");
    printf("Parent PID: %d\n", getpid());
    // Check if the correct number of arguments are provided
    if (argc != 4) {
        printf("Usage: %s <number of processes> <executable> <filename>\n", argv[0]);
        return 1;
    }



    int num_processes = atoi(argv[1]);

    for (int i = 0; i < num_processes; i++) {
        // Create a child process
        pid_t pid = fork();

        // Check if fork worked
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }

        // If this is the child process, execute simpleRead
        if (pid == 0) {
            char *args[] = {argv[2], argv[3], NULL}; //hardcoded the executable and filename
            execvp(args[0], args);

            // If exec returns, it means there was an error
            perror("exec failed");
            return 1;
        }
    }


    for (int i = 0; i < num_processes; i++) { //wait for all children to finish
        wait(NULL);
    }
    printf("All child processes completed.\n");
    printf("Parent PID: %d\n", getpid());

    return 0;
}