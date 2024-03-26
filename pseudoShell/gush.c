#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "histList.h"
#define SIGKILL 9

void gush_loop();
char* readLine();
char** divideLine(char* line); //parse the line into tokens and return an array of the tokens
void executeCommand(char** argumentsArray);
void pathExec(char** args);
void builtInExec(char** args);
//implementations of cd, exit, kill, history, pwd, path are auxilaries to builtInExec
void cdCommand(char** args);
void exitCommand();
void killCommand(char** args);
void historyCommand();
void pwdCommand(char** args);
void pathCommand(char** args);


const char error_message[30] = "An error has occurred\n";
const char* directories[] = {"/bin/", "/usr/bin/", NULL};
const char* builtInCommands[] = {"cd", "exit", "kill", "history", "pwd", "path", NULL};
const char* historyCommands[] = {"!1", "!2", "!3", "!4", "!5", "!6", "!7", "!8", "!9", "!10", "!11", "!12", "!13", "!14", "!15", "!16", "!17", "!18", "!19", "!20", NULL};
int isBuiltIn(char* cmd);
int isPathOrBuiltIn(char* cmd);

List historyList = {NULL, NULL, 0};
char* parseHistory(char** args);


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
        int cmdType = isBuiltIn(argsArr[0]);
        if (cmdType == 0){
            builtInExec(argsArr);
        } else if (cmdType == 1){
            pathExec(argsArr);
        }
        else if (cmdType == 2){
            char* execHist = parseHistory(argsArr);
            int cmdTypeHist = isPathOrBuiltIn(execHist);
            char** histArr = divideLine(execHist);
            if (cmdTypeHist == 0){
                builtInExec(histArr);
            } else if (cmdTypeHist == 1){
                pathExec(histArr);
            }
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
void executeCommand(char** args) {
    pid_t pid = fork();


    if (pid == -1) {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char* envp[] = { NULL };
        if (execve(args[0], args, envp) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }
    } else {
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
    int historyType = 2;
    for(int i = 0; builtInCommands[i] != NULL; i++){
        if(strcmp(cmd, builtInCommands[i]) == 0){
            return builtIN;
        }
    }
    for(int i = 0; historyCommands[i] != NULL; i++){
        if(strcmp(cmd, historyCommands[i]) == 0){
            printf("history command found\n");
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
void pathExec(char** argArr){
        for (int i = 0; directories[i] != NULL; i++) {
            char* path = malloc(strlen(directories[i]) + strlen(argArr[0]) + 1);
            strcpy(path, directories[i]);
            strcat(path, argArr[0]);
            if (access(path, X_OK) == 0) {
                argArr[0] = path;
                executeCommand(argArr);
                insertAtEnd(&historyList, argArr[0]);
            }
        }
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
        fprintf(stderr, "gush: expected argument to \"kill\"\n");
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
        //get the first element in the list
        //assign the first element of hist list to args[0] 
        if(searchList(&historyList, 0) == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else{
           char* cmd = searchList(&historyList, 0)->data;
            printf("first element in List: %s\n", cmd);
            return cmd;
        //execute the command 
        }
        
    }
    else if (strcmp(args[0], "!2") == 0){
        //get the second element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!3") == 0){
        //get the third element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!4") == 0){
        //get the fourth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!5") == 0){
        //get the fifth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!6") == 0){
        //get the sixth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!7") == 0){
        //get the seventh element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!8") == 0){
        //get the eighth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!9") == 0){
        //get the ninth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!10") == 0){
        //get the tenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!11") == 0){
        //get the eleventh element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!12") == 0){
        //get the twelveth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!13") == 0){
        //get the thirteenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!14") == 0){
        //get the fourteenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!15") == 0){
        //get the fifteenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!16") == 0){
        //get the sixteenth element in the list
        //execute the command
}
    else if (strcmp(args[0], "!17") == 0){
        //get the seventeenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!18") == 0){
        //get the eighteenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!19") == 0){
        //get the nineteenth element in the list
        //execute the command
    }
    else if (strcmp(args[0], "!20") == 0){
        //get the twentieth element in the list
        //execute the command
    }
    else {
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
}
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
void pathCommand(char** args){
    //implement path
}

