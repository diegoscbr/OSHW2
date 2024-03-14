#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CHARS 80

int main(int argc, char* argv[]){
    int pipefd[2];
    char buffer[MAX_CHARS + 1];
    pid_t pid;

    if (argc < 2 || argc > 4 || (argc == 3)) {
        fprintf(stderr, "Usage: %s <executable> <-o>  <outfile> \n", argv[0]);
        return EXIT_FAILURE;
    }



    if (pipe(pipefd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }



    pid = fork();
    if (pid == -1) { //pipe failed
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        if (argc == 4) {
            execlp(argv[1], argv[1], argv[2], argv[3], NULL); // Execute option
        } else {
            execlp(argv[1], argv[1], NULL); // Execute program1 without -o option
        }
        perror("execlp");
        return EXIT_FAILURE;
    }

     else {
        // Parent process
        close(pipefd[0]);
        printf("Parent PID: %d\n", getpid());

        fgets(buffer, MAX_CHARS, stdin);
        write(pipefd[1], buffer, strlen(buffer));
        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}