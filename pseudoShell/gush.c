#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SIGKILL 9

void gush_loop();
char* readLine();
char** divideLine(char* line); //parse the line into tokens and return an array of the tokens
void executeCommand(char** argumentsArray);
void pathCommand(char** args);
void builtInExec(char** args);
//implementations of cd, exit, kill, history, pwd, path are auxilaries to builtInExec

const char error_message[30] = "An error has occurred\n";
const char* directories[] = {"/bin/", "/usr/bin/", NULL};
const char* builtInCommands[] = {"cd", "exit", "kill", "history", "pwd", "path", NULL};
int isBuiltIn(char* cmd);


int main(int argc, char* argv[]){
//interactive mode
    gush_loop();
    return 0;
}





/*****************************/
/*****************************/
void gush_loop(){
    
    char* line; //line entered by user

    char** argsArr; //array of tokens

    int EXIT_FLG = 0;
    do{
        printf("gush>");
        //initially reado of line
        line = readLine();
        argsArr = divideLine(line); //parse the line into tokens array
        int cmdType = isBuiltIn(argsArr[0]);
        if (cmdType == 0){
            builtInExec(argsArr);
        } else {
            pathCommand(argsArr);
        }
        free(line);   
    } while (EXIT_FLG == 0);
}
/*****************************/
/*****************************/
/*SIMPLE READ INITIALLY*/
char* readLine() {
    char* line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin); //getline will allocate memory for line
    line[strcspn(line, "\n")] = 0; //removes the newline character from the end of the string
    return line;
}
/*****************************/
/*****************************/
/*PARSE THE LINE:
 ** into commands and arguments to be read*/
char** divideLine(char* line){
    int capacity = 10;
    int size = 0;

    char** tokArr = malloc(capacity * sizeof(char*));
    if (tokArr == NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
    }
    char* tokArrElement = strtok(line, " ");//get the first token

    while(tokArrElement != NULL){
        if(size >= capacity){
            capacity *= 2;
            tokArr = realloc(tokArr, capacity * sizeof(char*));
        if (tokArr == NULL){
            fprintf(stderr, "allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
    tokArr[size] = tokArrElement;
    size++;

    tokArrElement = strtok(NULL, " ");
    }
    if(size >= capacity){
        capacity *= 2;
        tokArr = realloc(tokArr, (capacity+1) * sizeof(char*));
        if (tokArr == NULL){
            fprintf(stderr, "allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
    tokArr[size] = NULL;
    return tokArr;  
}
/*****************************/
/*****************************/
void executeCommand(char** args) {
    // Fork a new process
    pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This is the child process. Execute the command.
        char* envp[] = { NULL }; // Empty environment for execve
        if (execve(args[0], args, envp) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // This is the parent process. Wait for the child to finish.
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}
/*****************************/
/*****************************/
int isBuiltIn(char* cmd){
    int builtIN = 0;
    int pathType = 1;
    for(int i = 0; builtInCommands[i] != NULL; i++){
        if(strcmp(cmd, builtInCommands[i]) == 0){
            return builtIN;
        }
    }
    return pathType;
}
/*****************************/
/*****************************/

/*****************************/
/*****************************/
void pathCommand(char** argArr){
        for (int i = 0; directories[i] != NULL; i++) {
            char* path = malloc(strlen(directories[i]) + strlen(argArr[0]) + 1);
            strcpy(path, directories[i]);
            strcat(path, argArr[0]);
            if (access(path, X_OK) == 0) {
                argArr[0] = path;
                executeCommand(argArr);
            }
        }
}
/*****************************/
/*****************************/
void builtInExec(char** args){
    if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message));
        } else {
            if(chdir(args[1]) != 0){
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }
    } else if(strcmp(args[0], "exit") == 0){
        exit(0);
    } else if(strcmp(args[0], "kill") == 0){
        if(args[1] == NULL){
            fprintf(stderr, "gush: expected argument to \"kill\"\n");
        } else {
            if(kill(atoi(args[1]), SIGKILL) != 0){
                perror("gush");
            }
        }
    } else if(strcmp(args[0], "history") == 0){
        //implement history
    } else if(strcmp(args[0], "pwd") == 0){
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s\n", cwd);
        } else {
            perror("gush");
        }
    } else if(strcmp(args[0], "path") == 0){
        //implement path
    }
}



