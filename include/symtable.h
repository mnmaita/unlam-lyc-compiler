#if !defined(_SYMTABLE_H_)
#define _SYMTABLE_H_

#include <cstdio>

struct SymtableRecord {
    char Name[32];
    char Type[12];
    int InternalType;
    char Value[100];
    int Length;
};

/* Symbol Table Variables */
extern char buffer[];
extern SymtableRecord SymbolTableASM[];
extern int SymbolTableASMEntries;

/* Symbol table functions */
int symtableSearch(char* Name);
int symtableSearchInternalType(char* Name);
int symtableInsert(int, char* Value);
int symtableInsertType(char* id, int Type);
int symtableWrite();

#endif // _SYMTABLE_H_
