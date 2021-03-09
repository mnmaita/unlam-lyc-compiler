#include "codegen.h"
#include "intermediate.h"
#include "list.h"
#include "log.h"
#include "symtable.h"
#include "types.h"
#include <cstring>

#define ASM_HEADER "cabecera.txt"
#define ASM_CODE "codigo.txt"
#define ASM_OUT "Final.asm"

List ASMElementsRead;

int GotoPosition[100];
int TotalGotos = 0;

int posInGotos(int pos) {
    int i = 0;
    while (i < TotalGotos) {
        if (GotoPosition[i] == pos) {
            return 1;
        }
        i++;
    }
    return 0;
}

int asmGenerateHeader() {
    FILE* pfASM;
    char buf[50];
    if ((pfASM = fopen(ASM_HEADER, "w")) == NULL) {
        logError(
            "[Generar Encabezado Assembler]: Error al generar el archivo de encabezado assembler");
    }
    fprintf(pfASM, "include src/asm/macros2.asm\ninclude src/asm/number.asm\n\n.MODEL "
                   "LARGE\n.STACK 200h\n.386\n\n.DATA\nMAXSTRSIZE equ 33\nTRUE dd 1\nFALSE dd "
                   "0\n@auxStr db MAXSTRSIZE dup (?), '$'\n");
    int i;
    for (i = 0; i < SymbolTableASMEntries; i++) {
        if (strcmp(SymbolTableASM[i].Type, "") != 0) {
            fprintf(pfASM, "%s %s %s\n", SymbolTableASM[i].Name, SymbolTableASM[i].Type,
                    SymbolTableASM[i].Value);
        } else {
            // strcpy(yytext, SymbolTableASM[i].Name);   // TODO: is this necessary?
            logError("Error al generar encabezado Assembler: El id no fue declarado previamente.");
        }
    }
    fclose(pfASM);
    return 1;
}

int asmGenerateCode() {
    FILE* pfASM;
    if ((pfASM = fopen(ASM_CODE, "w")) == NULL) {
        logError("[Generar Codigo Assembler]: Error al generar el archivo de codigo assembler");
    }
    fprintf(pfASM, "\n.CODE\n.startup\n\nmov AX,@DATA\nmov DS,AX\nFINIT\n\n");
    fclose(pfASM);
    return 1;
}

int asmWrite() {
    FILE* pfASM;
    FILE* pfCOD_ASM;
    FILE* pfCAB_ASM;
    char data[50];
    char filebuffer[4097];
    int n, k;
    if ((pfASM = fopen(ASM_OUT, "w")) == NULL) {
        logError("[Grabar Assembler]: Error al generar el archivo assembler final");
    }
    if ((pfCAB_ASM = fopen(ASM_HEADER, "a")) == NULL) {
        logError("[Grabar Assembler]: Error al querer editar el archivo cabecera assembler");
    }
    if ((pfCOD_ASM = fopen(ASM_CODE, "a")) == NULL) {
        logError("[Grabar Assembler]: Error al querer editar el archivo codigo assembler");
    }

    // Recorrer Polaca y transformar a assembler
    char* aux1;
    char* aux2;
    int pos = 1;
    while (irGetData(data, 1) == 1) {
        // Manejo de etiquetas
        if (posInGotos(pos) == 1) {
            fprintf(pfCOD_ASM, "GOTO%d: ", pos);
        }

        if (strcmp(data, "TRUE") == 0) {
            free(popStack(&polish));
            pushStack(&ASMElementsRead, "@aux");
            fprintf(pfCOD_ASM, "fld TRUE\n");
        } else if (strcmp(data, "FALSE") == 0) {
            free(popStack(&polish));
            pushStack(&ASMElementsRead, "@aux");
            fprintf(pfCOD_ASM, "fld FALSE\n");
        } else if (strstr(data, "GOTO") != NULL) {
            free(popStack(&polish));
            fprintf(pfCOD_ASM, "%s: \n", data);
        } else if (strcmp(data, "NOT") == 0 || strcmp(data, "AND") == 0) {
            // Ignoro el AND y NOT, pues no generan codigo assembler
            free(popStack(&polish));
        } else if (strcmp(data, "=") == 0 || strcmp(data, "+") == 0 || strcmp(data, "-") == 0 ||
                   strcmp(data, "*") == 0 || strcmp(data, "/") == 0 || strcmp(data, "CMP") == 0 ||
                   strcmp(data, "OR") == 0 || strcmp(data, "++") == 0) {
            // Operadores Binarios
            // Quitar el operador de la lista
            free(popStack(&polish));
            // Consumir dos operandos
            aux1 = dequeue(&ASMElementsRead);
            aux2 = dequeue(&ASMElementsRead);
            if (strcmp(data, "CMP") == 0) {
                if (strcmp(aux1, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux1);
                }
                if (strcmp(aux2, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                }
                fprintf(pfCOD_ASM, "fcomp\nfstsw ax\nfwait\nsahf\n", aux2, aux1);
            } else if (strcmp(data, "=") == 0) {
                if ((symtableSearchInternalType(aux2) == TYPE_STRING ||
                     symtableSearchInternalType(aux2) == TYPE_CONST_STRING) &&
                    (symtableSearchInternalType(aux1) == TYPE_STRING ||
                     symtableSearchInternalType(aux1) == TYPE_CONST_STRING)) {
                    if (symtableSearchInternalType(aux2) == TYPE_STRING ||
                        symtableSearchInternalType(aux2) == TYPE_CONST_STRING) {
                        fprintf(pfCOD_ASM,
                                "mov ax, @DATA\nmov ds, ax\nmov es, ax\nmov si,OFFSET _%s\nmov "
                                "di,OFFSET @auxStr\ncall COPIAR\n",
                                aux2);
                    }
                    if (symtableSearchInternalType(aux1) == TYPE_STRING) {
                        fprintf(pfCOD_ASM,
                                "mov ax, @DATA\nmov ds, ax\nmov es, ax\nmov si,OFFSET @auxStr\nmov "
                                "di,OFFSET _%s\ncall COPIAR\n",
                                aux1);
                    }
                } else if (symtableSearchInternalType(aux1) == TYPE_STRING &&
                           strcmp(aux2, "@aux") == 0) {
                    fprintf(pfCOD_ASM,
                            "mov ax, @DATA\nmov ds, ax\nmov es, ax\nmov si,OFFSET @auxStr\nmov "
                            "di,OFFSET _%s\ncall COPIAR\n",
                            aux1);
                } else {
                    if (strcmp(aux2, "@aux") != 0) {
                        fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                    }
                    fprintf(pfCOD_ASM, "fstp _%s\n", aux1);
                }
            } else if (strcmp(data, "+") == 0) {
                if (strcmp(aux2, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                }
                if (strcmp(aux1, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux1);
                }
                fprintf(pfCOD_ASM, "fadd\n", aux1);
            } else if (strcmp(data, "-") == 0) {
                if (strcmp(aux2, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                }
                if (strcmp(aux1, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux1);
                }
                fprintf(pfCOD_ASM, "fsub\n", aux1);
            } else if (strcmp(data, "*") == 0) {
                if (strcmp(aux2, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                }
                if (strcmp(aux1, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux1);
                }
                fprintf(pfCOD_ASM, "fmul\n", aux1);
            } else if (strcmp(data, "/") == 0) {
                if (strcmp(aux2, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux2);
                }
                if (strcmp(aux1, "@aux") != 0) {
                    fprintf(pfCOD_ASM, "fld _%s\n", aux1);
                }
                fprintf(pfCOD_ASM, "fdiv\n", aux1);
            } else if (strcmp(data, "++") == 0) {
                fprintf(
                    pfCOD_ASM,
                    "mov ax, @DATA\nmov ds, ax\nmov es, ax\nmov si,OFFSET _%s\nmov di,OFFSET "
                    "@auxStr\ncall COPIAR\nmov si,OFFSET _%s\nmov di,OFFSET @auxStr\ncall CONCAT\n",
                    aux2, aux1);
            } else if (strcmp(data, "OR") == 0) {
                // TODO: ver que accion tomar ante un OR en la polish, posiblemente
                // ignorarlo igual que AND y NOT
            }
            // Apilo el auxiliar correspondiente
            if (strcmp(data, "+") == 0 || strcmp(data, "-") == 0 || strcmp(data, "*") == 0 ||
                strcmp(data, "/") == 0 || strcmp(data, "CMP") == 0 || strcmp(data, "++") == 0) {
                pushStack(&ASMElementsRead, "@aux");
            }
            free(aux1);
            free(aux2);
        } else if (strcmp(data, "JL") == 0 || strcmp(data, "JLE") == 0 || strcmp(data, "JG") == 0 ||
                   strcmp(data, "JGE") == 0 || strcmp(data, "JZ") == 0 ||
                   strcmp(data, "JNZ") == 0 || strcmp(data, "JMP") == 0 ||
                   strcmp(data, "WRITE") == 0 || strcmp(data, "READ") == 0) {
            // Operadores Unarios
            free(popStack(&polish));
            aux1 = dequeue(&ASMElementsRead);
            // Link util sobre branches: https://en.wikipedia.org/wiki/Branch_(computer_science)
            if (strcmp(data, "JL") == 0) {
                fprintf(pfCOD_ASM, "JB GOTO%s\n", aux1);
            } else if (strcmp(data, "JLE") == 0) {
                fprintf(pfCOD_ASM, "JBE GOTO%s\n", aux1);
            } else if (strcmp(data, "JG") == 0) {
                fprintf(pfCOD_ASM, "JA GOTO%s\n", aux1);
            } else if (strcmp(data, "JGE") == 0) {
                fprintf(pfCOD_ASM, "JAE GOTO%s\n", aux1);
            } else if (strcmp(data, "JZ") == 0) {
                fprintf(pfCOD_ASM, "JZ GOTO%s\n", aux1);
            } else if (strcmp(data, "JNZ") == 0) {
                fprintf(pfCOD_ASM, "JNZ GOTO%s\n", aux1);
            } else if (strcmp(data, "JMP") == 0) {
                fprintf(pfCOD_ASM, "JMP GOTO%s\n", aux1);
            } else if (strcmp(data, "WRITE") == 0) {
                switch (symtableSearchInternalType(aux1)) {
                case TYPE_CONST_INT:
                case TYPE_INT:
                    fprintf(pfCOD_ASM, "DisplayFloat _%s,0\nnewLine\n", aux1);
                    break;
                case TYPE_CONST_FLOAT:
                case TYPE_FLOAT:
                    fprintf(pfCOD_ASM, "DisplayFloat _%s,3\nnewLine\n", aux1);
                    break;
                case TYPE_CONST_STRING:
                case TYPE_STRING:
                    fprintf(pfCOD_ASM, "displayString _%s\nnewLine\n", aux1);
                    break;
                }
            } else if (strcmp(data, "READ") == 0) {
                if (symtableSearchInternalType(aux1) == TYPE_STRING) {
                    fprintf(pfCOD_ASM, "getString _%s\n", aux1);
                } else if (symtableSearchInternalType(aux1) == TYPE_INT) {
                    // TODO
                    fprintf(pfCOD_ASM, "GetInteger _%s\n", aux1);
                } else if (symtableSearchInternalType(aux1) == TYPE_FLOAT) {
                    // TODO
                    fprintf(pfCOD_ASM, "GetFloat _%s\n", aux1);
                } else {
                    logError("Error al convertir a Assembler: Tipo erroneo para la variable usada "
                             "para la operacion READ");
                }
            }
            if (strcmp(data, "JL") == 0 || strcmp(data, "JLE") == 0 || strcmp(data, "JG") == 0 ||
                strcmp(data, "JGE") == 0 || strcmp(data, "JZ") == 0 || strcmp(data, "JNZ") == 0 ||
                strcmp(data, "JMP") == 0) {
                GotoPosition[TotalGotos++] = atoi(aux1);
            }
            free(aux1);
        } else {
            pushStack(&ASMElementsRead, popStack(&polish));
        }
        pos++;
    }
    // agrega el goto final
    if (posInGotos(pos) == 1) {
        fprintf(pfCOD_ASM, "GOTO%d: ", pos);
    }

    fclose(pfCOD_ASM);
    fclose(pfCAB_ASM);

    FILE* pfCabecera = fopen(ASM_HEADER, "r");
    if (pfCabecera == NULL) {
        logError("[Grabar Assembler]: Error al abrir el archivo cabecera assembler");
    }
    FILE* pfCodigo = fopen(ASM_CODE, "r");
    if (pfCodigo == NULL) {
        logError("[Grabar Assembler]: Error al abrir el archivo codigo assembler");
    }
    while ((n = fread(filebuffer, sizeof(char), 4096, pfCabecera))) {
        if (!(k = fwrite(filebuffer, sizeof(char), n, pfASM))) {
            return -1;
        }
    }

    while ((n = fread(filebuffer, sizeof(char), 4096, pfCodigo))) {
        if (!(k = fwrite(filebuffer, sizeof(char), n, pfASM))) {
            return -1;
        }
    }

    fclose(pfCabecera);
    fclose(pfCodigo);
    remove(ASM_HEADER);
    remove(ASM_CODE);
    fprintf(pfASM, "mov AX, 4C00h\nint 21h\n\n");
    FILE* pfConcat = fopen("src/asm/concat.asm", "r");
    if (pfConcat == NULL) {
        logError("[Grabar Assembler]: Error al abrir el archivo concat assembler");
    }
    while ((n = fread(filebuffer, sizeof(char), 4096, pfConcat))) {
        if (!(k = fwrite(filebuffer, sizeof(char), n, pfASM))) {
            return -1;
        }
    }
    fprintf(pfASM, "\nEND\n");
    fclose(pfASM);
    return 1;
}
