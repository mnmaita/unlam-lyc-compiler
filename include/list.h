#ifndef INCLUDE_LIST_H_
#define INCLUDE_LIST_H_

#include <string>

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
int listInsert(List* List, std::string Data, int Pos);
int listInsertBack(List* List, ListElement* Current, std::string Data);
ListElement* listGetStartElement(List* List);
ListElement* listGetEndElement(List* List);
int listSetData(List* List, char* NewData, int Pos);
int listGetData(List* List, char* Data, int Pos);

/* Stack and Queue functions */
void pushStack(List* List, std::string Data);
char* popStack(List* List);
char* dequeue(List* List);

#endif // INCLUDE_LIST_H_
