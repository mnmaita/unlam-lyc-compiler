#if !defined(_INTERMEDIATE_H_)
#define _INTERMEDIATE_H_

#include "list.h"

extern List polish;

void irInsert(char* data);
void irSetData(char* data, int pos);
int irGetData(char* data, int pos);
void irInsertAt(char* data, int pos);
int irWrite();

#endif // _INTERMEDIATE_H_
