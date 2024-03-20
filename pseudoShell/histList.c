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

void insertAtEnd(Node** headRef, char* data) {
    Node* newNode = createNode(data);
    if (*headRef == NULL) {
        *headRef = newNode;
    } else {
        Node* temp = *headRef;
        while (temp->next != *headRef) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->next = *headRef; // Make it circular
    }
}

void printList(Node* head) {
    if (head != NULL) {
        Node* temp = head;
        do {
            printf("%s\n", temp->data);
            temp = temp->next;
        } while (temp != head);
    }
}

void freeList(Node** headRef) {
    Node* temp = *headRef;
    Node* next;
    do {
        next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
    } while (temp != *headRef);
    *headRef = NULL;
}
int main(){
    Node* head = NULL;
    insertAtEnd(&head, "ls");
    insertAtEnd(&head, "cd");
    insertAtEnd(&head, "pwd");
    insertAtEnd(&head, "echo");
    insertAtEnd(&head, "cat");
    insertAtEnd(&head, "ls");
    insertAtEnd(&head, "cd");
    insertAtEnd(&head, "pwd");
    insertAtEnd(&head, "echo");
    insertAtEnd(&head, "cat");
    printList(head);
    freeList(&head);
    return 0;
}
