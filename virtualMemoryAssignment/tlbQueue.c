#include "tlbQueue.h"
#include <stdio.h>
#include <stdlib.h>

struct Queue* createQueue(int capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int**)malloc(queue->capacity * sizeof(int*));
    for (int i = 0; i < capacity; ++i) {
        queue->array[i] = (int*)malloc(2 * sizeof(int));
    }
    return queue;
}

int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

int* dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    int* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void enqueue(struct Queue* queue, int pageNumber, int frameNumber)
{
    if (isFull(queue)) {
        int* item = dequeue(queue);
        if (item != NULL) {
            free(item);
        }
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear][0] = pageNumber;
    queue->array[queue->rear][1] = frameNumber;
    queue->size = queue->size + 1;
}

void printQueue(struct Queue* queue)
{
    int count = 0;
    int i = queue->front;
    while (count < queue->size) {
        printf("Page: %d Frame: %d\n", queue->array[i][0], queue->array[i][1]);
        i = (i + 1) % queue->capacity;
        count++;
    }
}

int* getFront(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->front];
}

int* getRear(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->rear];
}

