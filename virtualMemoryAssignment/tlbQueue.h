#ifndef QUEUE_H
#define QUEUE_H

struct Queue {
    int front, rear, size;
    unsigned capacity;
    int** array;
};

struct Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
int* dequeue(struct Queue* queue);
void enqueue(struct Queue* queue, int pageNumber, int frameNumber);
void printQueue(struct Queue* queue);
int* getFront(struct Queue* queue);
int* getRear(struct Queue* queue);

#endif /* QUEUE_H */
