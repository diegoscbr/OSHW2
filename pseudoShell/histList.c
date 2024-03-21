#include "histList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Node* createNode(char* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = strdup(data);
    newNode->next = newNode; // Point to itself initially
    newNode->histBit = 0;
    return newNode;
}
void setHistBit(Node* entry) {
    entry->histBit = 1;
}
void insertAtEnd(List* list, char* data) {
    Node* newNode = createNode(data);
    if (list->size == 0) {
        list->head = newNode;
        list->tail = newNode;
        list->size++;
    } else if (list->size < MAX_ENTRIES) {
        list->tail->next = newNode;
        list->tail = newNode;
        list->size++;
    } else {
        Node* oldHead = list->head;
        list->head = oldHead->next;
        oldHead->next = NULL;
        free(oldHead->data);
        free(oldHead);
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->tail->next = list->head; // Make it circular
}
void printList(List* list) {
    if (list->head != NULL) {
        Node* temp = list->head;
        int i = 1;
        do {
            printf("%d. ", i);

            printf("%s\n", temp->data);
            temp = temp->next;
            i++;
        } while (temp != list->head);
    }
}
void freeList(List* list) {
    Node* temp = list->head;
    Node* next;
    do {
        next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
    } while (temp != list->head);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
/*
int main(){
    List list = {NULL, NULL, 0}; // Initialize list
    for (int i = 0; i < 25; i++) {
        char data[10];
        sprintf(data, "cmd%d", i);
        insertAtEnd(&list, data);
    }
    printList(&list);
    freeList(&list);
    return 0;
}
*/
