%{
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Compiler.tab.h"

#include "intermediate.h"
#include "symtable.h"
#include "declares.h"
#include "codegen.h"
#include "header.h"
#include "types.h"
#include "log.h"

/* yacc/Bison Variables */
extern int yylineno;
extern int yystopparser=0;
extern FILE *yyin;
extern char *yyltext;
extern char *yytext;


int yyerror(char* message) {
	fprintf(stderr, "Syntax Error (line %d): %s -> %s\n", yylineno, message, yytext);
	system("Pause");
	exit(-1);
}

extern int yylex(void);

%}

%union {
	int intval;
	double realval;
	char * strval;
}

%token BEGIN_PROGRAM END_PROGRAM
%token DECLARE AS
%token CONST_INT CONST_FLOAT CONST_STR
%token REAL INTEGER STRING
%token IF THEN ELSE END_IF
%token DO WHILE END_WHILE
%token ID
%token OP_ASSIGN
%token OP_AND OP_OR OP_NOT
%token OP_EQUAL OP_NOTEQUAL OP_LESSER OP_LESSEREQUAL OP_GREATER OP_GREATEREQUAL
%token OP_ADD OP_SUB OP_MULT OP_DIV
%token OP_CONCAT
%token FN_EQUALS FN_ALLEQUAL
%token COMMA SEMICOLON
%token PARENTHESES_OPEN PARENTHESES_CLOSE BRACKETS_OPEN BRACKETS_CLOSE
%token IO_WRITE IO_READ

%%
programa:
	BEGIN_PROGRAM 	{
						listInit(&polish);
						listInit(&Selections);
						asmGenerateCode();
					}
	bloque_declaraciones
	bloque
	END_PROGRAM {
					printf("Ending program...\n");
					symtableWrite();
					irWrite();
					printf("GRABE POLACA \n");
					asmGenerateHeader();
					printf("GRABE ENCABEZADO ASM \n");
					asmWrite();
					// TODO: clear function to delete symbol table and intermediate code
					printf("Compilation was successful.\n");
				}
	| BEGIN_PROGRAM 	{
							listInit(&polish);
							listInit(&Selections);
							asmGenerateCode();
						}
	  bloque
	  END_PROGRAM	{
		  				printf("Ending program...\n");
						symtableWrite();
						irWrite();
						printf("GRABE POLACA \n");
						asmGenerateHeader();
						printf("GRABE ENCABEZADO ASM \n");
						asmWrite();
						printf("Compilation was successful.\n");
					}
					
	| BEGIN_PROGRAM END_PROGRAM
	;

bloque:	sentencia
		| bloque sentencia
		;

bloque_declaraciones:	declaracion SEMICOLON { insertar_declares_en_TS(); }
						| bloque_declaraciones declaracion SEMICOLON { insertar_declares_en_TS(); }
						;

sentencia:	ciclo
			| seleccion
			| asignacion SEMICOLON
			| entrada SEMICOLON
			| salida SEMICOLON
			;

ciclo:	WHILE	{
					char buf[50];
					strcpy(buf,"GOTO");
					strcat(buf,itoa(polish.size+2, buffer, 10));
					irInsert(buf);
					pushStack(&Cycles, itoa(polish.size+1, buffer, 10));
					IsIfCondition = 0;
				}
		PARENTHESES_OPEN condicion PARENTHESES_CLOSE	DO bloque
		END_WHILE	{
						irSetData(itoa(polish.size+3,buffer,10),atoi(dequeue(&Cycles)));
						if($<intval>4 > 1)
							irSetData(itoa(polish.size+3,buffer,10),atoi(dequeue(&Cycles)));
						irInsert(dequeue(&Cycles));
						irInsert("JMP");
					}
		;

seleccion:	IF {IsIfCondition = 1;} bloque_if
			;
	
bloque_if:	PARENTHESES_OPEN condicion PARENTHESES_CLOSE THEN bloque
			END_IF	{
						irSetData(itoa(polish.size+1,buffer,10),atoi(dequeue(&Selections)));
						if($<intval>2 > 1)
							irSetData(itoa(polish.size+1,buffer,10),atoi(dequeue(&Selections)));
					}

			| PARENTHESES_OPEN condicion PARENTHESES_CLOSE THEN bloque
			  ELSE	{
						irSetData(itoa(polish.size+3,buffer,10),atoi(dequeue(&Selections)));
						if($<intval>2 > 1)
							irSetData(itoa(polish.size+3,buffer,10),atoi(dequeue(&Selections)));
						pushStack(&Selections, itoa(polish.size+1,buffer,10));
						irInsert("RES2");
						irInsert("JMP");
					}
			  bloque END_IF	{
								irSetData(itoa(polish.size+1,buffer,10), atoi(dequeue(&Selections)));
							}
			;
			

condicion:	comparacion {
							if(IsIfCondition == 1)
								pushStack(&Selections, itoa(polish.size+2,buffer,10));
							else if(IsIfCondition == 0)
								pushStack(&Cycles, itoa(polish.size+2,buffer,10));
							irInsert("CMP");
							irInsert("RES1");
							irInsert($<strval>1);
							$<intval>$ = 1; //La regla devuelve la cantidad de comparaciones involucradas
						}
			| OP_NOT comparacion	{
										if(IsIfCondition == 1)
											pushStack(&Selections, itoa(polish.size+2,buffer,10));
										else if(IsIfCondition == 0)
											pushStack(&Cycles, itoa(polish.size+2,buffer,10));
										irInsert("CMP");
										irInsert("RES1");
										char jump[5];
										if(strcmp($<strval>2, "JNZ") == 0)
											strcpy(jump,"JZ");
										else if(strcmp($<strval>2, "JZ") == 0)
											strcpy(jump,"JNZ");
										else if(strcmp($<strval>2, "JLE") == 0)
											strcpy(jump,"JG");
										else if(strcmp($<strval>2, "JL") == 0)
											strcpy(jump,"JGE");
										else if(strcmp($<strval>2, "JGE") == 0)
											strcpy(jump,"JL");
										else if(strcmp($<strval>2, "JG") == 0)
											strcpy(jump,"JLE");
										irInsert(jump);
										irInsert("NOT");
										$<intval>$ = 1; //La regla devuelve la cantidad de comparaciones involucradas
									}
			| comparacion	{
								if(IsIfCondition == 1)
									pushStack(&Selections, itoa(polish.size+2,buffer,10));
								else if(IsIfCondition == 0)
									pushStack(&Cycles, itoa(polish.size+2,buffer,10));
								irInsert("CMP");
								irInsert("RES1");
								irInsert($<strval>1);
							}
			  OP_AND comparacion	{
										if(IsIfCondition == 1)
											pushStack(&Selections, itoa(polish.size+2,buffer,10));
										else if(IsIfCondition == 0)
											pushStack(&Cycles, itoa(polish.size+2,buffer,10));
										irInsert("CMP");
										irInsert("RES1");
										irInsert($<strval>4);
										irInsert("AND");
										$<intval>$ = 2; //La regla devuelve la cantidad de comparaciones involucradas
									}
			| comparacion	{
								if(IsIfCondition == 1)
									pushStack(&Selections, itoa(polish.size+2,buffer,10));
								else if(IsIfCondition == 0)
									pushStack(&Cycles, itoa(polish.size+2,buffer,10));
								irInsert("CMP");
								irInsert("RES1");
								irInsert($<strval>1);
							}
			  OP_OR comparacion	{
									if(IsIfCondition == 1)
										pushStack(&Selections, itoa(polish.size+2,buffer,10));
									else if(IsIfCondition == 0)
										pushStack(&Cycles, itoa(polish.size+2,buffer,10));
									irInsert("CMP");
									irInsert("RES1");
									irInsert($<strval>4);
									irInsert("OR");
									$<intval>$ = 2; //La regla devuelve la cantidad de comparaciones involucradas
								}
			| all_equal	{
							if(IsIfCondition == 1)
								pushStack(&Selections, itoa(polish.size+2,buffer,10));
							else if(IsIfCondition == 0)
								pushStack(&Cycles, itoa(polish.size+2,buffer,10));
							irInsert("CMP");
							irInsert("RES1");
							irInsert("JNZ");
							$<intval>$ = 1; //La regla devuelve la cantidad de comparaciones involucradas
						}
			;

asignacion:	ID	{
					if(symtableSearch(yytext) < 0)
						yyerror("No se encuentra el id en la tabla de simbolos.");
					else
						symtableInsert(TYPE_ID, yytext);
					AssignmentType = symtableSearchInternalType(yytext);
				}

			OP_ASSIGN expresion	{
									if(AssignmentType == $<intval>4 || AssignmentType+100 == $<intval>4) {
										irInsert($<strval>1);
										irInsert("=");
									}
									else {
										yyerror("Tipos incompatibles al realizar la asignacion");
									}
									
								}
			;

entrada:
	IO_READ ID	{ symtableInsert(TYPE_ID, yytext); irInsert($<strval>2); irInsert("READ"); } ;

salida:
	IO_WRITE factor {irInsert("WRITE");};

declaracion:
	DECLARE { printf("Declaracion: "); } BRACKETS_OPEN lista_variables_declaracion BRACKETS_CLOSE AS BRACKETS_OPEN lista_tipos_declaracion BRACKETS_CLOSE ;

lista_variables_declaracion:	ID 	{ symtableInsert(TYPE_ID,yylval.strval); declare_insert(yylval.strval); }
								| ID { symtableInsert(TYPE_ID,yylval.strval); declare_insert(yylval.strval); } COMMA lista_variables_declaracion 
								;

lista_tipos_declaracion:	type 
							| lista_tipos_declaracion COMMA type 
							;

type:	INTEGER		{ declare_insert_type(TYPE_INT); }
		| REAL		{ declare_insert_type(TYPE_FLOAT); }
		| STRING	{ declare_insert_type(TYPE_STRING); }
		;

comparacion:	expresion OP_EQUAL expresion			{ $<strval>$ = "JNZ"; }
				| expresion OP_NOTEQUAL expresion		{ $<strval>$ = "JZ"; }
				| expresion OP_GREATER expresion 			{ $<strval>$ = "JLE"; }
				| expresion OP_GREATEREQUAL expresion 	{ $<strval>$ = "JL"; }
				| expresion OP_LESSER expresion			{ $<strval>$ = "JGE"; }
				| expresion OP_LESSEREQUAL expresion 	{ $<strval>$ = "JG"; }
				;

expresion:	termino { $<intval>$ = $<intval>1; }
			| expresion OP_ADD  termino 	{
												if( ($<intval>1 != TYPE_CONST_STRING && $<intval>1 != TYPE_STRING) && ($<intval>3 != TYPE_CONST_STRING && $<intval>3 != TYPE_STRING) ) {
													if( ($<intval>1 == TYPE_CONST_INT || $<intval>1 == TYPE_INT) && ($<intval>3 == TYPE_CONST_INT || $<intval>3 == TYPE_INT) ||
														($<intval>1 == TYPE_CONST_FLOAT || $<intval>1 == TYPE_FLOAT) && ($<intval>3 == TYPE_CONST_FLOAT || $<intval>3 == TYPE_FLOAT) ) {
															irInsert("+");
													}
													else {
														yyerror("Tipos incompatibles al sumar");
													}
												}
												else {
													yyerror("Los factores a sumar no pueden ser variables o constantes de tipo String");
												}
											}
			| expresion OP_SUB termino 	{
												if( ($<intval>1 != TYPE_CONST_STRING && $<intval>1 != TYPE_STRING) && ($<intval>3 != TYPE_CONST_STRING && $<intval>3 != TYPE_STRING) ) {
													if( ($<intval>1 == TYPE_CONST_INT || $<intval>1 == TYPE_INT) && ($<intval>3 == TYPE_CONST_INT || $<intval>3 == TYPE_INT) ||
														($<intval>1 == TYPE_CONST_FLOAT || $<intval>1 == TYPE_FLOAT) && ($<intval>3 == TYPE_CONST_FLOAT || $<intval>3 == TYPE_FLOAT) ) {
															irInsert("-");
													}
													else {
														yyerror("Tipos incompatibles al restar");
													}
												}
												else {
													yyerror("Los factores a restar no pueden ser variables o constantes de tipo String");
												}
											}
			| factor OP_CONCAT factor	 	{
												if( ($<intval>1 == TYPE_CONST_STRING || $<intval>1 == TYPE_STRING) && ($<intval>3 == TYPE_CONST_STRING || $<intval>3 == TYPE_STRING) ) {
													irInsert("++");
												}
												else {
													yyerror("Los factores en una concatenacion deben ser variables o constantes de tipo String");
												}												
											}
			;

termino:	factor	{ $<intval>$ = $<intval>1; }
			| termino OP_MULT factor	{
													if( ($<intval>1 != TYPE_CONST_STRING && $<intval>1 != TYPE_STRING) && ($<intval>3 != TYPE_CONST_STRING && $<intval>3 != TYPE_STRING) ) {
														if( ($<intval>1 == TYPE_CONST_INT || $<intval>1 == TYPE_INT) && ($<intval>3 == TYPE_CONST_INT || $<intval>3 == TYPE_INT) ||
															($<intval>1 == TYPE_CONST_FLOAT || $<intval>1 == TYPE_FLOAT) && ($<intval>3 == TYPE_CONST_FLOAT || $<intval>3 == TYPE_FLOAT) ) {
																irInsert("*");
														}
														else {
															yyerror("Tipos incompatibles al multiplicar");
														}
													}
													else {
														yyerror("Los factores a multiplicar no pueden ser variables o constantes de tipo String");
													}
												}
			| termino OP_DIV factor	{
												if( ($<intval>1 != TYPE_CONST_STRING && $<intval>1 != TYPE_STRING) && ($<intval>3 != TYPE_CONST_STRING && $<intval>3 != TYPE_STRING) ) {
													if( ($<intval>1 == TYPE_CONST_INT || $<intval>1 == TYPE_INT) && ($<intval>3 == TYPE_CONST_INT || $<intval>3 == TYPE_INT) ||
														($<intval>1 == TYPE_CONST_FLOAT || $<intval>1 == TYPE_FLOAT) && ($<intval>3 == TYPE_CONST_FLOAT || $<intval>3 == TYPE_FLOAT) ) {
															irInsert("/");
													}
													else {
														yyerror("Tipos incompatibles al dividir");
													}
												}
												else {
													yyerror("Los factores a dividir no pueden ser variables o constantes de tipo String");
												}
											}
			;	
	
factor:	ID	{
				if(symtableSearch(yytext) < 0)
					yyerror("No se encuentra el id en la tabla de simbolos.");
				else {
					irInsert(yytext);
					$<intval>$ = symtableSearchInternalType(yytext);
				}
				//symtableInsert(TYPE_ID, yytext);
			}

		| CONST_INT		{
							symtableInsert(TYPE_CONST_INT,yytext);
							irInsert(yytext);
							$<intval>$ = TYPE_CONST_INT;
						}

		| CONST_FLOAT	{
							symtableInsert(TYPE_CONST_FLOAT,yytext);
							irInsert(yytext);
							$<intval>$ = TYPE_CONST_FLOAT;
						}

		| CONST_STR 	{
							symtableInsert(TYPE_CONST_STRING,yytext);
							irInsert(yytext);
							$<intval>$ = TYPE_CONST_STRING;
						}
		| iguales
		;

lista_expresiones_allequal:	expresion_allequals
							| lista_expresiones_allequal COMMA expresion_allequals
							;
	
lista_expresiones_iguales:	expresion_iguales
							| lista_expresiones_iguales COMMA expresion_iguales
							;

expresion_iguales:	expresion	{
						char buf[40];
						pushStack(&EqualsList,itoa(polish.size+3,buffer,10));
						strcpy(buf,"_expr_iguales");
						strcat(buf,itoa(EqualsIndex,buffer,10));
						irInsert(buf);
						irInsert("CMP");
						irInsert("RES");
						irInsert("JNZ");
						strcpy(buf,"_cont_iguales");
						strcat(buf,itoa(EqualsIndex,buffer,10));
						irInsert(buf);
						irInsert("1");
						irInsert("+");
						irInsert(buf);
						irInsert("=");
						irSetData(itoa(polish.size+1,buffer,10),atoi(popStack(&EqualsList)));
					}
					;
	
expresion_allequals:	expresion	{
							char buf[40];
							if (TotalExpressionLists == 0) {
								TotalExpressionAllEquals++;
								strcpy(buf,"_expr");
								strcat(buf,itoa(TotalExpressionAllEquals,buffer,10));
								strcat(buf,"_alleq");
								strcat(buf,itoa(AllequalIndex,buffer,10));
								symtableInsert(TYPE_FLOAT,buf);
								irInsert(buf);
								irInsert("=");
								pushStack(&AllEqualsExpressions,itoa(polish.size+1,buffer,10));
							}
							else {
								int pos = atoi(dequeue(&AllEqualsExpressions));
								TotalExpressionAllEquals++;
								strcpy(buf,"_expr");
								strcat(buf,itoa(TotalExpressionAllEquals,buffer,10));
								strcat(buf,"_alleq");
								strcat(buf,itoa(AllequalIndex,buffer,10));
								irInsertAt(buf,pos);
								irInsertAt("CMP",pos+1);
								irInsertAt("RES",pos+2);
								pushStack(&AllEqualsStack,itoa(pos+3,buffer,10));
								irInsertAt("JNZ",pos+3);	
								pushStack(&AllEqualsExpressions,itoa(pos+5,buffer,10));						
							}
						}
						;

lista_expresiones_iguales_corchetes:
	BRACKETS_OPEN lista_expresiones_iguales BRACKETS_CLOSE ;

lista_expresiones_allequal_corchetes:
	BRACKETS_OPEN lista_expresiones_allequal BRACKETS_CLOSE ;

listas_expresiones_allequal:	lista_expresiones_allequal_corchetes { TotalExpressionLists = 1; TotalExpressionAllEquals=0; }
								| listas_expresiones_allequal COMMA lista_expresiones_allequal_corchetes { TotalExpressionLists++; TotalExpressionAllEquals=0; }
								;

all_equal:	FN_ALLEQUAL { AllequalIndex++; TotalExpressionAllEquals = 0;} PARENTHESES_OPEN listas_expresiones_allequal 
			{
				if(TotalExpressionLists<2) 
					yyerror("AllEqual debe recibir como parametro al menos dos listas.");
				irInsert("TRUE");
				irInsert(itoa(polish.size+4,buffer,10));
				irInsert("JMP");
				irInsert("FALSE");
				irInsert("TRUE");
				while(AllEqualsStack.size > 0){
					irSetData(itoa(polish.size-1,buffer,10),atoi(popStack(&AllEqualsStack)));
				}
			}
			PARENTHESES_CLOSE
			; // Returns true if the expression is equal to all of the list elements, otherwise it returns false

iguales:	FN_EQUALS { EqualsIndex++; }
			PARENTHESES_OPEN expresion 	{
										char buf[40];
										strcpy(buf,"_expr_iguales");
										strcat(buf,itoa(EqualsIndex,buffer,10));
										symtableInsert(TYPE_FLOAT,buf);
										irInsert(buf);
										strcpy(buf,"_cont_iguales");
										strcat(buf,itoa(EqualsIndex,buffer,10));		
										symtableInsert(TYPE_INT,buf);
										irInsert("=");
									}
			COMMA lista_expresiones_iguales_corchetes PARENTHESES_CLOSE	{
																		char buf[40];
																		strcpy(buf,"_cont_iguales");
																		strcat(buf,itoa(EqualsIndex,buffer,10));
																		irInsert(buf);
																	}
			; // Returns the total count of equal elements on a list

%%

/* MAIN */
int main(int argc,char *argv[]) {
	if ((yyin = fopen(argv[1], "rt")) == NULL) {
		printf("Unable to open file: %s\n", argv[1]);
		return -1;
	}
	else {
		yyparse();
	}
	fclose(yyin);
	return 0;
}
