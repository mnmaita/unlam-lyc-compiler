/* FUNCIONES DE NOTACION POLACA INVERSA */
#include "intermediate.h"
#include "list.h"
#include "log.h"
#include <cstdio>

#define IR_STRUCTURE "ir_structure.txt" // Intermediate Representation

List polish;

void irInsert(char* data) {
    char* ptr = data;
    while (*ptr != '\0') {
        if (*ptr == ' ' || *ptr == '"' || *ptr == '.' || *ptr == ',' || *ptr == ';' ||
            *ptr == ':' || *ptr == '!' || *ptr == '?')
            *ptr = '_';
        ptr++;
    }
    listInsertBack(&polish, polish.End, data);
}

void irSetData(char* data, int pos) { listSetData(&polish, data, pos); }

int irGetData(char* data, int pos) { return listGetData(&polish, data, pos); }

void irInsertAt(char* data, int pos) {
    listInsert(&polish, data, pos);
    // if(!listInsert(&polish, data, pos))
    // printf("Insercion en posicion %d de la polish exitosa.", pos);
}

int irWrite() {
    int i;
    FILE* pfPolaca;
    if ((pfPolaca = fopen(IR_STRUCTURE, "w")) == NULL) {
        logError("[Guardar Polaca]: Error al generar la polish");
    }
    ListElement* current = polish.Start;
    for (i = 0; i < polish.Size && current != NULL; i++) {
        fprintf(pfPolaca, " %s ", current->Data);
        current = current->Next;
    }
    fclose(pfPolaca);
}
