#include "symtable.h"
#include "log.h"
#include "types.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define SYMTABLE "ts.txt"

// Assembler Symbol table internal functions

int asmSymtableInsert(char* Value, int Type) {
    if ((asmSymtableSearch(Value)) == -1) {
        SymtableRecord reg;
        strcpy(reg.Name, "_");
        strcat(reg.Name, Value);
        strcpy(reg.Value, "");
        strcpy(reg.Type, "");
        reg.Length = strlen(Value);
        SymbolTableASM[SymbolTableASMEntries++] = reg;
        // Pudo insertar en SYMTABLE
        return 1;
    }
    // printf("No se pudo insertar el ID %s en cabecera ASM.\n",Value);
    // No pudo insertar
    return 0;
}

int asmSymtableInsertType(char* id, int Type) {
    int pos = asmSymtableSearch(id);
    int tsPos = symtableSearch(id);
    if (pos >= 0) {
        if (Type == TYPE_INT || Type == TYPE_FLOAT || Type == TYPE_CONST_INT ||
            Type == TYPE_CONST_FLOAT) {
            strcpy(SymbolTableASM[pos].Type, "dd");
            if (Type == TYPE_INT || Type == TYPE_FLOAT) {
                strcpy(SymbolTableASM[pos].Value, "?");
            } else {
                strcpy(SymbolTableASM[pos].Value, SymbolTable[tsPos].Value);
                if (Type == TYPE_CONST_INT) {
                    strcat(SymbolTableASM[pos].Value, ".0");
                }
            }
        } else if (Type == TYPE_STRING || Type == TYPE_CONST_STRING) {
            strcpy(SymbolTableASM[pos].Type, "db");
            if (Type == TYPE_STRING) {
                strcpy(SymbolTableASM[pos].Value, "MAXSTRSIZE dup (?), '$'");
            } else {
                strcat(SymbolTableASM[pos].Value, SymbolTable[tsPos].Value);
                strcat(SymbolTableASM[pos].Value, ", '$', ");
                strcat(SymbolTableASM[pos].Value,
                       itoa(strlen(SymbolTable[tsPos].Value) + 1, buffer, 10));
                strcat(SymbolTableASM[pos].Value, " dup (?)");
            }
        } else if (Type == TYPE_ID) {
            strcpy(SymbolTableASM[pos].Type, "");
        } else {
            logError("[Insertar Tipo ASM]: Tipo invalido; No se puede insertar el tipo pasado "
                     "por parametro.");
        }
    } else {
        logError("[Insertar Tipo ASM]: No se encontro el id en la tabla de simbolos.");
    }
    return 1;
}

int asmSymtableSearch(char* Name) {
    int i, index = 0;
    for (i = 0; i < SymbolTableASMEntries; i++) {
        if (SymbolTableASM[i].Name[0] == '@' || SymbolTableASM[i].Name[0] == '_') {
            index = 1;
        } else {
            index = 0;
        }
        // printf("Comparando %s con %s\n",SymbolTableASM[i].Name+index,Name);
        if (strcmp(SymbolTableASM[i].Name + index, Name) == 0) {
            return i;
        }
    }
    return -1;
}

// Symbol Table Variables
FILE* pf_Symtable;
char buffer[1000];
SymtableRecord SymbolTable[1000];
int SymbolTableEntries = 0;

SymtableRecord SymbolTableASM[1000];
int SymbolTableASMEntries = 0;

// TABLA DE SIMBOLOS
int symtableSearch(char* Name) {
    int i, index = 0;
    for (i = 0; i < SymbolTableEntries; i++) {
        if (SymbolTable[i].Name[0] == '_') {
            index = 1;
        } else {
            index = 0;
        }
        // printf("Comparando %s con %s\n",SymbolTable[i].Name+index,Name);
        if (!strcmp(SymbolTable[i].Name + index, Name)) {
            return i;
        }
    }
    return -1;
}

int symtableSearchInternalType(char* Name) {
    int i = symtableSearch(Name);
    return i >= 0 ? SymbolTable[i].InternalType : -1;
}

int symtableInsert(int Type, char* Value) {
    char buf[100];
    strcpy(buf, Value);
    if (symtableSearch(Value) == -1) {
        SymtableRecord reg;
        if (Type == TYPE_CONST_INT || Type == TYPE_CONST_FLOAT || Type == TYPE_CONST_STRING) {
            char* ptr = Value;
            while (*ptr != '\0') {
                if (*ptr == ' ' || *ptr == '"' || *ptr == '.' || *ptr == ',' || *ptr == ';' ||
                    *ptr == ':' || *ptr == '!' || *ptr == '?')
                    *ptr = '_';
                ptr++;
            }
            asmSymtableInsert(Value, Type);
            strcpy(reg.Name, "_");
            strcat(reg.Name, Value);
            strcpy(reg.Value, buf);
            if (Type == TYPE_CONST_INT) {
                strcpy(reg.Type, "CONST_INT");
                reg.InternalType = TYPE_CONST_INT;
            } else if (Type == TYPE_CONST_FLOAT) {
                strcpy(reg.Type, "CONST_FLOAT");
                reg.InternalType = TYPE_CONST_FLOAT;
            } else if (Type == TYPE_CONST_STRING) {
                strcpy(reg.Type, "CONST_STR");
                reg.InternalType = TYPE_CONST_STRING;
            }
        } else if (Type == TYPE_ID || Type == TYPE_INT || Type == TYPE_FLOAT ||
                   Type == TYPE_STRING) {
            asmSymtableInsert(Value, Type);
            strcpy(reg.Name, Value);
            strcpy(reg.Value, "");
            if (Type == TYPE_ID) {
                strcpy(reg.Type, "");
                reg.InternalType = TYPE_ID;
            } else {
                if (Type == TYPE_INT) {
                    strcpy(reg.Type, "Integer");
                    reg.InternalType = TYPE_INT;
                } else if (Type == TYPE_FLOAT) {
                    strcpy(reg.Type, "Real");
                    reg.InternalType = TYPE_FLOAT;
                } else if (Type == TYPE_STRING) {
                    strcpy(reg.Type, "String");
                    reg.InternalType = TYPE_STRING;
                }
            }
        } else {
            logError("[Insertar SYMTABLE]: Tipo invalido; No se puede insertar un elemento "
                     "del tipo pasado por parametro.");
        }
        reg.Length = strlen(Value);
        SymbolTable[SymbolTableEntries++] = reg;
        asmSymtableInsertType(Value, Type);
        // printf("ID %s insertado en SYMTABLE exitosamente.\n",Value);
        // Pudo insertar en SYMTABLE
        return 1;
    }
    // printf("No se pudo insertar el ID %s en SYMTABLE.\n",Value);
    // No pudo insertar
    return 0;
}

int symtableInsertType(char* id, int Type) {
    int pos = symtableSearch(id);
    // printf("Posicion en SYMTABLE: %d",pos);
    if (pos >= 0) {
        if (Type == TYPE_INT) {
            strcpy(SymbolTable[pos].Type, "Integer");
            SymbolTable[pos].InternalType = TYPE_INT;
        } else if (Type == TYPE_FLOAT) {
            strcpy(SymbolTable[pos].Type, "Real");
            SymbolTable[pos].InternalType = TYPE_FLOAT;
        } else if (Type == TYPE_STRING) {
            strcpy(SymbolTable[pos].Type, "String");
            SymbolTable[pos].InternalType = TYPE_STRING;
        } else {
            logError("[Insertar Tipo]: Tipo invalido; No se puede insertar el tipo pasado por "
                     "parametro.");
        }
        asmSymtableInsertType(id, Type);
    } else {
        logError("[Insertar Tipo]: No se encontro el id en la tabla de simbolos.");
    }
    return 1;
}

int symtableWrite() {
    printf("Writing SYMTABLE...");
    int i;
    if ((pf_Symtable = fopen(SYMTABLE, "w")) == NULL) {
        logError("[Grabar SYMTABLE]: Error al generar la tabla de simbolos.");
        return -1;
    }
    fprintf(pf_Symtable, "NOMBRE \t\t TIPO \t\t VALOR \t\t LONGITUD \n");
    for (i = 0; i < SymbolTableEntries; i++) {
        fprintf(pf_Symtable, "%s \t\t", SymbolTable[i].Name);
        if (SymbolTable[i].Type != NULL) {
            fprintf(pf_Symtable, "%s \t\t", SymbolTable[i].Type);
        }
        if (SymbolTable[i].Value != NULL) {
            fprintf(pf_Symtable, "%s \t\t", SymbolTable[i].Value);
        }
        fprintf(pf_Symtable, "%d \n", SymbolTable[i].Length);
    }
    fclose(pf_Symtable);
    printf("SYMTABLE successfully written.\n");
    return 0;
}
