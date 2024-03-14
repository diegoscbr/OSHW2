#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 3) {
        printf("Usage: %s <./executable> <file.txt>\n", argv[0]);
        return 1;
    }

    // Create a child process
    pid_t pid = fork();

    // Check if fork was successful
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }
    if (pid > 0) {
        printf("This is the Parent Process\n");
        printf("Parent PID: %d\n", getpid());
    }

    // If this is the child process, execute the simpleRead program
    if (pid == 0) {
        printf("This is the Child Process. PID:");
        printf("%d\n", getpid());
        char *args[] = {argv[1], argv[2], NULL};
        execvp(args[0], args);

        // If exec returns, it means there was an error
        perror("exec failed");
        return 1;
    }

    // If this is the parent process, wait for the child to finish
    wait(NULL);
    printf("child process completed.\n");
    printf("Ending Parent Process. PID:  \n");
    printf("%d\n", getpid());
    return 0;
}