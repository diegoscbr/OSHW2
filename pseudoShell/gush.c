#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void gush_loop();
char* readLine();


int main(int argc, char* argv[]){

/*Program must:
*1. Read cmd frm stdIN
* 2. Parse cmd string into the shell command + its arguments
* 3. Execute the parse command
*/


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
        if(line == "exit"){
            exit(0);
        }
        
    } while (EXIT_FLG);
}

char* readLine(){
    char *line = NULL;
    size_t bufsize = 0; // Initial buffer size (0 means getline will allocate)
    ssize_t characters_read;

    // Read a line of input from the user
    printf("Enter a line of text: ");
    characters_read = getline(&line, &bufsize, stdin);

    if (characters_read == -1) {
        // An error occurred or end-of-file was reached
        perror("getline failed");
        exit(EXIT_FAILURE);
    }

    return line;
}
