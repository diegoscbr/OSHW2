#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include "histList.h"
#define SIGKILL 9
#define MAX_LINE 1024

void gush_loop();
char* readLine();
char** divideLine(char* line); 
void executeCommand(char** argumentsArray);
void pathExec(char** args, char* cmd);
char* getFullCommand(char** cmd);
void builtInExec(char** args);
//implementations of cd, exit, kill, history, pwd, path are auxilaries to builtInExec
void cdCommand(char** args);
void exitCommand();
void killCommand(char** args);
void historyCommand();
void pwdCommand(char** args);
void pathCommand(char** args);
List historyList = {NULL, NULL, 0};



const char error_message[30] = "An error has occurred\n";

const char* builtInCommands[] = {"cd", "exit", "kill", "history", "pwd", "path", NULL};
const char* historyCommands[] = {"!1", "!2", "!3", "!4", "!5", "!6", "!7", "!8", "!9", "!10", "!11", "!12", "!13", "!14", "!15", "!16", "!17", "!18", "!19", "!20", NULL};
int isBuiltIn(char* cmd);
int isPathOrBuiltIn(char* cmd);
int findRedirectionOperator(char** args);
int* findBothIndex(char** args);
int containsRedirectionOperator(char** args);
int argArrayLength(char** args);
//List historyList = {NULL, NULL, 0};
char* parseHistory(char** args);

char* directories[] = {"/bin", NULL};
//char* directories[] = {"/bin/", "/usr/bin/", "/usr/local/bin", NULL};
/*****************************/
/*****************************/
int main(int argc, char* argv[]){
    if (argv[1] != NULL){
        //read from file
    }
    else{
       gush_loop(); 
    }
    
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
        if (argsArr == NULL || argsArr[0] == NULL)
        {
            free(line);
            continue;
        }
        int cmdType = isBuiltIn(argsArr[0]);
        if (cmdType == 0){
            builtInExec(argsArr);
        } else if (cmdType == 1){
            pathExec(argsArr, argsArr[0]);
        }
        else if (cmdType == 2){
            char* execHist = parseHistory(argsArr);
            char** histArr = divideLine(execHist);
            int cmdTypeHist = isPathOrBuiltIn(histArr[0]);
            if (cmdTypeHist == 0){
                builtInExec(histArr);
            } else if (cmdTypeHist == 1){
                printf("histArr[0]:\n"); //output /bin/ls
                printf("%s\n", histArr[0]);
                pathExec(histArr, histArr[0]);
                printf("should have executed\n");
            }
            free(histArr);
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
    getline(&line, &bufsize, stdin); 
    line[strcspn(line, "\n")] = 0; 
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
void handleOutputRedirection(char** args, int redirIndex) {
    char* target = strdup(args[redirIndex + 1]);
    int fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDOUT_FILENO) == -1){
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
    args[redirIndex] = NULL;
}

void handleInputRedirection(char** args, int redirIndex) {
    char* source = strdup(args[redirIndex + 1]);
    int fd = open(source, O_RDONLY, 0777);
    if (fd < 0){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDIN_FILENO) == -1){
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
    args[redirIndex] = NULL;
}
/*****************************/
/*****************************/
void handleInputandOutputRedirection(char** args, int* redirIndecies){
 //remove redirection operators
 int fd_input, fd_output;
 fd_input = open(args[(redirIndecies[0]) + 1], O_RDONLY);
 fd_output = open(args[(redirIndecies[1]) + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
 if (fd_input < 0 || fd_output < 0){
     write(STDERR_FILENO, error_message, strlen(error_message));
     exit(EXIT_FAILURE);
 }
 if(dup2(fd_input, STDIN_FILENO) == -1 || dup2(fd_output, STDOUT_FILENO) == -1){
     write(STDERR_FILENO, error_message, strlen(error_message));
     exit(EXIT_FAILURE);
 }
 close(fd_input);
 close(fd_output);

}
/*****************************/
/*****************************/
char ** removeRedirectionOperators(char** args, int* redirIndex) {
    int j = 0;
    char** newArgs = malloc(sizeof(char*) * MAX_LINE/2+1);
    for (int i = 0; args[i] != NULL; i++) {
        if (i != redirIndex[0] && i != redirIndex[0] + 1 && i != redirIndex[1] && i != redirIndex[1] + 1) {
            newArgs[j] = args[i];
            j++;
        }
    }
    newArgs[j] = NULL;
    free(args);
    args = newArgs;
    return args;
}
/*****************************/
/*****************************/
int getPipeIndex(char** args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            return i;
        }
    }
    return -1;
}
/*****************************/
/*****************************/
void executeCommand(char** args) {
    pid_t pid = fork();
    if (pid < 0) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
    } else if (pid == 0) { //child process
        char* envp[] = {NULL};
        int check = containsRedirectionOperator(args);
        int redirIndex = findRedirectionOperator(args);

        if (check == 1) {
            handleOutputRedirection(args, redirIndex);
        } else if (check == 2) { // stdin redirection
            handleInputRedirection(args, redirIndex);
        } else if (check == 3) {
            int *both = findBothIndex(args);
            handleInputandOutputRedirection(args, both);
            args = removeRedirectionOperators(args, both);
            free(both);
        }  else if (check == 4){
            //handleSinglePipe(args);
            int fd[2];
            int pipeIndex = getPipeIndex(args);


            if (pipe(fd) == -1) {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(EXIT_FAILURE);
            }
            int pid1 = fork();
            if (pid1 < 0){
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(EXIT_FAILURE); 
            }
            if (pid1 == 0){
                //get contents of array before pipe
                char** leftArgs = (char**)malloc(sizeof(char*) * (pipeIndex));;
                for (int i = 0; i < pipeIndex; i++){
                    leftArgs[i] = args[i];
                    printf("leftArgs[i]: %s\n", leftArgs[i]);
                }
                leftArgs[pipeIndex] = NULL;

                printf("contents of leftArgs:");
                for(int i = 0; leftArgs[i] != NULL; i++){
                    printf("%s\n", leftArgs[i]);
                }
                 printf("calling execve leftArgs[0]: %s\n", leftArgs[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
               
                if(execve(leftArgs[0], leftArgs, envp) == -1){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(EXIT_FAILURE);
                }
            }//end of pid1 == 0

            int pid2 = fork();
            if (pid2 < 0){
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(EXIT_FAILURE); 
            }
            if (pid2 == 0){
                //get contents of array after pipe
                char **rightArgs = (char**)malloc(sizeof(char*) * (argArrayLength(args) - pipeIndex));
               int j = 0;
                for (int i = pipeIndex + 1; args[i] != NULL; i++){
                    rightArgs[j] = args[i];
                    printf("rightArgs[j]: %s\n", rightArgs[j]);
                    j++;
                }
                rightArgs[j] = NULL;
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                printf("calling execve rightArgs[0]: %s\n", rightArgs[0]);
                if(execve(rightArgs[0], rightArgs, envp) == -1){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(EXIT_FAILURE);
                }

            }
            close(fd[0]);
            close(fd[1]);

            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }//end of check == 4 

        //calls execve if no redirection to stdout
        if ( execve(args[0], args, envp) == -1) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(EXIT_FAILURE);
        }
    } else 
     {
        waitpid(pid, NULL, 0);
     }
}
/*****************************/
/*****************************/
int isBuiltIn(char* cmd){
    int builtIN = 0;
    int pathType = 1;
    int historyType = 2;
    for(int i = 0; builtInCommands[i] != NULL; i++){
        if(strcmp(cmd, builtInCommands[i]) == 0){
            return builtIN;
        }
    }
    for(int i = 0; historyCommands[i] != NULL; i++){
        if(strcmp(cmd, historyCommands[i]) == 0){
            //printf("history command found\n");
            return historyType;
        }
    }

    return pathType;
}

/*****************************/
/*****************************/
int isPathOrBuiltIn(char* cmd){ //checks just path or built in 
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
void pathExec(char** argArr, char* cmd){
        for (int i = 0; directories[i] != NULL; i++) {
            char* path = malloc(strlen(directories[i]) + strlen(cmd) + 1);
            strcpy(path, directories[i]);
            strcat(path, "/");
            strcat(path, cmd);
            if (access(path, X_OK) == 0) {
                argArr[0] = path;
                executeCommand(argArr);
                insertAtEnd(&historyList, getFullCommand(argArr));
            }
            else{
                free(path);
            }
        }
}

char* getFullCommand(char** cmd){
    char* fullCmd = strdup(cmd[0]);
    for(int i = 1; cmd[i] != NULL; i++){
                fullCmd = realloc(fullCmd, strlen(fullCmd) + strlen(cmd[i]) + 2); // +2 for the space and null terminator
                strcat(fullCmd, " ");
                strcat(fullCmd, cmd[i]);
            }
    return fullCmd;
}
/*****************************/
/*****************************/
void builtInExec(char** args){
    char * cmdWithArg;
    if (args[1] != NULL){
        cmdWithArg = malloc(strlen(args[0]) + strlen(args[1]) + 2);
        strcpy(cmdWithArg, args[0]);
        strcat(cmdWithArg, " ");
        strcat(cmdWithArg, args[1]);
    }
    if(strcmp(args[0], "cd") == 0){
        insertAtEnd(&historyList, cmdWithArg);
        cdCommand(args);
    } else if(strcmp(args[0], "exit") == 0){
        insertAtEnd(&historyList, "exit");
        exitCommand();
    } else if(strcmp(args[0], "kill") == 0){
        insertAtEnd(&historyList, cmdWithArg);
        killCommand(args);
    } else if(strcmp(args[0], "history") == 0){
        historyCommand();
    } else if(strcmp(args[0], "pwd") == 0){
        insertAtEnd(&historyList, "pwd");
        pwdCommand(args);
    } else if(strcmp(args[0], "path") == 0){
        insertAtEnd(&historyList, "path");
        pathCommand(args);
    }
}
/*****************************/
/*****************************/
void cdCommand(char** args){
    if(args[1] == NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        if(chdir(args[1]) != 0){   
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
}
/*****************************/
/*****************************/
void exitCommand(){
    exit(0);
}
/*****************************/
/*****************************/
void killCommand(char** args){
    if(args[1] == NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        if(kill(atoi(args[1]), SIGKILL) != 0){
            perror("gush");
        }
    }
}
/*****************************/
/*****************************/
void historyCommand(){
    printList(&historyList);
}

char* parseHistory(char** args){
    if(strcmp(args[0], "!1") == 0){
        if(searchList(&historyList, 0) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
           char* cmd = searchList(&historyList, 0)->data;
           printf("%s\n", cmd);
            return cmd;
        }      
    }
    else if(strcmp(args[0], "!2") == 0){
        if(searchList(&historyList, 1) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
           char* cmd = searchList(&historyList, 1)->data;
           printf("%s\n", cmd);
            return cmd;
        }      
    }
    else if (strcmp(args[0], "!3") == 0){
       if(searchList(&historyList, 2) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
           char* cmd = searchList(&historyList, 2)->data;
           printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!4") == 0){
       if(searchList(&historyList, 3) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
           char* cmd = searchList(&historyList, 3)->data;
           printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!5") == 0){
        if(searchList(&historyList, 4) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 4)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!6") == 0){
        if(searchList(&historyList, 5) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 5)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!7") == 0){
        if(searchList(&historyList, 6) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 6)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!8") == 0){
        if(searchList(&historyList, 7) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 7)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!9") == 0){
        if(searchList(&historyList, 8) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 8)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else if (strcmp(args[0], "!10") == 0){
        if(searchList(&historyList, 9) == 0){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
            char* cmd = searchList(&historyList, 9)->data;
            printf("%s\n", cmd);
            return cmd;
        }    
    }
    else{
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
}//end of parseHistory

/*****************************/
/*****************************/
void pwdCommand(char** args){
    char current[1024];
    if (args[1] != NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else{
        if(getcwd(current, sizeof(current)) != NULL){
        printf("%s\n", current);
        }else {write(STDERR_FILENO, error_message, strlen(error_message));}
    } 
}
/*****************************/
/*****************************/
void clearDirectories(char* list){
    for(int i = 0; i < strlen(list); i++){
        free(list[i]);
        list[i] = NULL;
    }
}
/*****************************/
/*****************************/
void pathCommand(char** args){
    //implement path
    if (args[1] == NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
        clearDirectories(directories);
    }

}

/*****************************/
/*****************************/
int findRedirectionOperator(char** args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], "<") == 0){
            return i;
        }
    }
    return -1;
}
/*****************************/
/*****************************/
int argArrayLength(char** args){
    int i = 0;
    while(args[i] != NULL){
        i++;
    }
    return i;
}
/*****************************/
/*****************************/
int containsRedirectionOperator(char** args){
    int inputRedirect = 0;
    int outputRedirect = 0;
    int pipeRedirect = 0;
    for (int i = 0; args[i] != NULL; i++) {
       if (strcmp(args[i], ">") == 0) {
            outputRedirect = 1;
        } //stdin case
        else if (strcmp(args[i], "<") == 0) {
            inputRedirect = 1;
        }//stdout and std and redirection 
        else if (strcmp(args[i], "|") == 0) {
            pipeRedirect = 1;
        }
    }
    if(inputRedirect == 1 && outputRedirect == 1){
        return 3;
    }
    else if(inputRedirect == 1){
        return 2;
    }
    else if(outputRedirect == 1){
        return 1;
    }
    else if(pipeRedirect == 1){
        return 4; //pipe case
    }
    return 0; //error case
}
/*****************************/
/*****************************/
int* findBothIndex(char** args) {
    int STDIN = -1; 
    int STDOUT = -1; 
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            STDIN = i; // Update STDIN index
        }
    }
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], ">") == 0) {
            STDOUT = j; 
        }
    }
    int* both = (int*)malloc(2 * sizeof(int));
    if (both == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    both[0] = STDIN;
    both[1] = STDOUT;
    return both;
}
    