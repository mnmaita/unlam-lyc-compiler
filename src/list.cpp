#include "list.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

List Selections;
List Cycles;

void listInit(List* List) {
    List->Start = NULL;
    List->End = NULL;
    List->Size = 0;
}

/* Inserts an element on the list in the desired position */
int listInsert(List* List, std::string Data, int Pos) {
    if (List->Size < 2) {
        printf("[listInsert]: Error: list size is less than two.\n");
        return -1;
    }

    if (Pos < 1 || Pos >= List->Size) {
        if (Pos == List->Size) {
            if (listInsertBack(List, List->End, Data) == 0) {
                return 0;
            }
        } else {
            printf("[listInsert]: Error: the desired insertion position is out of bounds.\n");
            return -1;
        }
    }

    ListElement* Current;
    ListElement* NewElement;

    if ((NewElement = (ListElement*)malloc(sizeof(ListElement))) == NULL) {
        printf("[listInsert]: Error: bad pointer to element allocation.\n");
        return -1;
    }

    if ((NewElement->Data = (char*)malloc(50 * sizeof(char))) == NULL) {
        printf("[listInsert]: Error: bad data allocation.\n");
        return -1;
    }

    Current = List->Start;
    for (int i = 1; i < Pos; ++i) {
        Current = Current->Next;
    }
    if (Current->Next == NULL) {
        printf("[listInsert]: Error: the next element is NULL.\n");
        return -1;
    }

    strcpy(NewElement->Data, Data.c_str());

    NewElement->Next = Current->Next;
    Current->Next = NewElement;
    List->Size++;
    return 0;
}

/* Inserts an element at the back of the list */
int listInsertBack(List* List, ListElement* current, std::string Data) {
    ListElement* NewElement;

    if ((NewElement = (ListElement*)malloc(sizeof(ListElement))) == NULL) {
        return -1;
    }
    if ((NewElement->Data = (char*)malloc(strlen(Data.c_str()) * sizeof(char) + 1)) == NULL) {
        return -1;
    }

    strcpy(NewElement->Data, Data.c_str());
    NewElement->Next = NULL;

    if (List->Size == 0) {
        List->Start = NewElement;
        List->End = NewElement;
    } else {
        current->Next = NewElement;
        List->End = NewElement;
    }

    List->Size++;
    return 0;
}

/* Gets the first element of the list and removes it */
ListElement* listGetStartElement(List* List) {
    if (List->Size == 0) {
        return NULL;
    }
    ListElement* AuxElement;
    AuxElement = List->Start;
    List->Start = List->Start->Next;
    if (List->Size == 1) {
        List->End = NULL;
    }
    List->Size--;
    return AuxElement;
}

ListElement* listGetEndElement(List* List) {
    if (List->Size == 0) {
        return NULL;
    }
    ListElement *AuxElement, *AuxFinalElement;
    if (List->Size > 1) {
        AuxElement = List->Start;
        for (int i = 1; i < List->Size - 1; ++i) {
            AuxElement = AuxElement->Next;
        }
        AuxFinalElement = AuxElement->Next;
        AuxElement->Next = NULL;
        List->End = AuxElement;
    } else {
        AuxFinalElement = List->Start;
    }
    List->Size--;
    return AuxFinalElement;
}

int listSetData(List* List, char* NewData, int Pos) {
    if (List->Size == 0 || Pos > List->Size) {
        return -1;
    }
    ListElement* Current;
    Current = List->Start;
    for (int i = 1; i < Pos; ++i) {
        Current = Current->Next;
    }
    free(Current->Data);
    if ((Current->Data = (char*)malloc(strlen(NewData) * sizeof(char) + 1)) == NULL) {
        return -1;
    }
    strcpy(Current->Data, NewData);
    return 1;
}

int listGetData(List* List, char* Data, int Pos) {
    if (List->Size == 0 || Pos > List->Size) {
        return -1;
    }
    ListElement* Current;
    Current = List->Start;
    for (int i = 1; i < Pos; ++i) {
        Current = Current->Next;
    }
    strcpy(Data, Current->Data);
    return 1;
}

void pushStack(List* List, std::string Data) {
    if (List == &Selections || List == &Cycles)
        printf("Apilando POS %s\n", Data);
    listInsertBack(List, List->End, Data);
}

char* popStack(List* List) { // FIXME
    char* Data = listGetStartElement(List)->Data;
    if (List == &Selections || List == &Cycles)
        printf("Desapilando (desacolando) POS %s\n", Data);
    return Data;
}

char* dequeue(List* List) { // FIXME
    char* Data = listGetEndElement(List)->Data;
    if (List == &Selections || List == &Cycles)
        printf("Desacolando (desapilando) POS %s\n", Data);
    return Data;
}
