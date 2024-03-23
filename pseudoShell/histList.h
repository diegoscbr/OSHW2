#ifndef HISTLIST_H
#define HISTLIST_H
#define MAX_ENTRIES 20
typedef struct Node {
    int histBit;
    char* data;
    struct Node* next;
} Node;
typedef struct List {
    Node* head;
    Node* tail;
    int size;
} List;
void setHistBit(Node* entry);
void insertAtEnd(List* list, char* data);
Node* createNode(char* data);
void printList(List* list);
void freeList(List* list);
void searchList(char *searchFlg, int entryNum, List* list);

#endif /* HISTLIST_H */