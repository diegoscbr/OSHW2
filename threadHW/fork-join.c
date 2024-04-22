#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

sem_t s; 

void *child(void *arg) {
   
    Sem_wait(&s);
    printf("child\n");

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    // init semaphore here
    Sem_init(&s, 0);
    Pthread_create(&p, NULL, child, NULL);
    // use semaphore here
    sleep(1);
    Sem_post(&s);
    Pthread_join(p, NULL);
   
    
    
    printf("parent: end\n");
    
    return 0;
}

