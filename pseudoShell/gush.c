#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void gush_loop();
char* readLine();


int main(int argc, char* argv[]){
//interactive mode
    gush_loop();
    return 0;
}

void gush_loop(){
    
    char* line;
    int EXIT_FLG = 0;
    do{
        printf("gush>");
        line = readLine();
        if(strcmp(line,"exit") == 0){
            EXIT_FLG = 1;
        }
        free(line);
        
    } while (EXIT_FLG == 0);
}

char* readLine() {
    char* line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    line[strcspn(line, "\n")] = 0; //removes the newline character from the end of the string
    return line;
}

