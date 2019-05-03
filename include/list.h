#ifndef _LIST_H_
#define _LIST_H_

struct ListElement {
    char* Data;
    ListElement* Next;
};

struct List {
    ListElement* Start;
    ListElement* End;
    int Size;
};

extern List Selections;
extern List Cycles;

void listInit(List* List);
int listInsert(List* List, char* Data, int Pos);
int listInsertBack(List* List, ListElement* Current, char* Data);
ListElement* listGetStartElement(List* List);
ListElement* listGetEndElement(List* List);
int listSetData(List* List, char* NewData, int Pos);
int listGetData(List* List, char* Data, int Pos);

/* Stack and Queue functions */
void pushStack(List* List, char* Data);
char* popStack(List* List);
char* dequeue(List* List);

#endif