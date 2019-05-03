#include "declares.h"
#include "log.h"
#include "symtable.h"
#include <cstring>

struct DeclareRecord {
    char Name[32];
    int Type;
};

DeclareRecord Declares[500];
int DeclaresIndex = 0;
int DeclaresTypeIndex = 0;

int declare_insert(char* var) {
    strcpy(Declares[DeclaresIndex].Name, var);
    return ++DeclaresIndex;
}

int declare_insert_type(int Type) {
    Declares[DeclaresTypeIndex].Type = Type;
    return ++DeclaresTypeIndex;
}

int insertar_declares_en_TS() {
    int i = 0;
    if (DeclaresIndex == DeclaresTypeIndex) {
        for (i; i < DeclaresIndex; i++) {
            symtableInsertType(Declares[i].Name, Declares[i].Type);
        }
        DeclaresIndex = 0;
        DeclaresTypeIndex = 0;
    } else {
        logError("La cantidad de IDs en la lista del declare es distinta a la cantidad de tipos.");
        return -1;
    }
    return 0;
}
