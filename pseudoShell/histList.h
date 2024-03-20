#ifndef HISTLIST_H
#define HISTLIST_H
#define MAX_ENTRIES 20
typedef struct Node {
    int histBit;
    char* data;
    struct Node* next;
} Node;
void setHistBit(Node* entry);
void insertAtEnd(Node** headRef, char* data);
Node* createNode(char* data);
void printList(Node* head);
void freeList(Node** headRef);

#endif /* HISTLIST_H */