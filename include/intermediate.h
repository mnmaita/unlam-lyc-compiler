#ifndef INCLUDE_INTERMEDIATE_H_
#define INCLUDE_INTERMEDIATE_H_

#include "list.h"

extern List polish;

void irInsert(char* data);
void irSetData(char* data, int pos);
int irGetData(char* data, int pos);
void irInsertAt(char* data, int pos);
int irWrite();

#endif // INCLUDE_INTERMEDIATE_H_
