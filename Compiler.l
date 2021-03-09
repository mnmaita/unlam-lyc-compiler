%{
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include "Compiler.tab.h"

/*#define TYPE_INT 101
#define TYPE_FLOAT 102
#define TYPE_STRING 103
#define TYPE_ID 104*/

#define MIN_INT -32768
#define MAX_INT 32767
#define MIN_FLOAT -2147483648.f
#define MAX_FLOAT 2147483647.f
#define MAX_STRING 30
#define MAX_ID 15

extern FILE *yyin;
extern YYSTYPE yylval;

char lexbuffer[1000];

int validateInteger(int Value);
int validateFloat(float Value);
int validateString(char * str);
int validateID(char * id);
void lexError(char* message);
%}

%option noyywrap
%option yylineno

%%
"beginprogram"	{ printf(" # Begin Program\n"); return BEGIN_PROGRAM; }
"endprogram"	{ printf(" # End Program\n"); return END_PROGRAM; }
"declare"		{ printf(" # Declare\n"); return DECLARE; }
"as"			{ printf(" # As\n"); return AS; }

"Real"			{ printf(" # Float Type\n"); return REAL; }
"Integer"		{ printf(" # Integer Type\n"); return INTEGER; }
"String"		{ printf(" # String Type\n"); return STRING; }

"if"			{ printf(" # If\n"); return IF; }
"then"   		{ printf(" # Then\n"); return THEN; }
"else"   		{ printf(" # Else\n"); return ELSE; }
"endif"	      	{ printf(" # End If\n"); return END_IF; }

"do"	      	{ printf(" # Do\n"); return DO; }
"while"			{ printf(" # While "); return WHILE; }
"endwhile" 		{ printf(" # End While\n"); return END_WHILE; }

"="				{ printf(" # Operator =\n"); return OP_ASSIGN; }
"+"				{ printf(" # Operator +\n"); return OP_ADD; }
"-"				{ printf(" # Operator -\n"); return OP_SUB; }
"*"				{ printf(" # Operator *\n"); return OP_MULT; }
"/"				{ printf(" # Operator /\n"); return OP_DIV; }
">"             { printf(" # Operator >\n"); return OP_GREATER; }
">="            { printf(" # Operator >=\n"); return OP_GREATEREQUAL; }
"<"             { printf(" # Operator <\n"); return OP_LESSER; }
"<="            { printf(" # Operator <=\n"); return OP_LESSEREQUAL; }
"!="            { printf(" # Operator !=\n"); return OP_NOTEQUAL; }
"=="            { printf(" # Operator ==\n"); return OP_EQUAL; }
"++"			{ printf(" # Operator ++\n"); return OP_CONCAT; }
"and"           { printf(" # Operator and\n"); return OP_AND; }
"or"            { printf(" # Operator or\n"); return OP_OR; }
"not"           { printf(" # Operator not\n"); return OP_NOT; }

"#Iguales"		{ printf(" # #Equals Function\n"); return FN_EQUALS; }
"AllEqual"		{ printf(" # AllEqual Function\n"); return FN_ALLEQUAL; }

"("				{ printf(" # Parentheses Open\n"); return PARENTHESES_OPEN; }
")"				{ printf(" # Parentheses Close\n"); return PARENTHESES_CLOSE; }
"["				{ printf(" # Brackets Open\n"); return BRACKETS_OPEN; }
"]"				{ printf(" # Brackets Close\n"); return BRACKETS_CLOSE; }

","				{ printf(" # Comma (,)\n"); return COMMA; }
";"				{ printf(" # Semicolon (;)\n"); return SEMICOLON; }

"write"			{ printf(" # Output (Write)\n"); return IO_WRITE; }
"read"			{ printf(" # Input (Read)\n"); return IO_READ; }

[a-zA-Z]([a-zA-Z]|[0-9]|_)*		{	/* ID */
									yylval.strval = (char*)malloc(strlen(yytext) + 1);
									strcpy(yylval.strval, yytext);
									printf(" # ID: %s ",yylval.strval);
									if(validateID(yylval.strval))
										return ID;
								}

-?([1-9][0-9]+|[0-9])	{	/* INTEGER CONSTANT */
							if(strlen(yytext) > 6)
								lexError("Integer constant size exceeded (Maximum 6 characters long)");
							yylval.intval = atoi(yytext);
							printf(" # Integer Constant: %d ",yylval.intval);
							if(validateInteger(yylval.intval))
								return CONST_INT;
						}

\"[^"\r\n]+\"	{	/* STRING CONSTANT */
					yylval.strval = (char*)malloc(strlen(yytext) + 1);
					strcpy(yylval.strval, yytext);
					printf(" # String Constant: %s ", yylval.strval);
					if(validateString(yylval.strval))
						return CONST_STR;
				}

-?(([0-9]|[1-9][0-9]+)\.[0-9]*|[.][0-9]+)	{	/* FLOAT CONSTANT */
												if(strlen(yytext) > 20)
													lexError("Float constant size exceeded (Maximum 20 characters long)");
												yylval.realval = atof(yytext);
												printf(" # Float Constant: %f ", yylval.realval);
												if(validateFloat(yylval.realval))
													return CONST_FLOAT;
											}
(--\/.*)	{ printf(" # Single line comment\n"); }
(-\/[^-/]*)?-\/([^-/])*\/-([^-/]*\/-)?	{ printf(" # Multiline comment\n"); }
-\/	{ lexError("Multiline comment has no closing character."); }
\/-	{ lexError("Multiline comment has no opening character."); }
[ \t\n\r]+		{ /* Ignores whitespace */ }

.	{ lexError("Invalid character."); }
%%

int validateString(char * str) {
    if(strlen(str) <= (int)MAX_STRING) {
		return 1;
	}        
    else {
		fprintf(stderr, "LEXICAL ERROR (line %d): String size exceeded (Maximum %d characters long) -> \%s\n", yylineno, (int)MAX_STRING, str);
        system("PAUSE");
        exit(-1);
    }
}

int validateInteger(int Value) {
    if(Value <= (int)MAX_INT && Value >= (int)MIN_INT) {
        return 1;
	}        
    else {
        fprintf(stderr, "LEXICAL ERROR (line %d): Integer constant out of range (Allowed range: from %d to %d) -> \%d\n", yylineno, (int)MIN_INT, (int)MAX_INT, Value);
        system("PAUSE");
        exit(-1);
    }
}

int validateFloat(float Value) {
	if(Value > (float)MAX_FLOAT || Value < (float)MIN_FLOAT) {
		fprintf(stderr, "LEXICAL ERROR (line %d): Float constant out of range (Allowed range: from %f to %f) -> \%f\n", yylineno, (float)MIN_FLOAT, (float)MAX_FLOAT, atof(yytext));
        system("PAUSE");	
		exit(-1);
	}
	return 1;
}

int validateID(char * id) {
    if(strlen(id) <= (int)MAX_ID) {
		return 1;
	}        
    else {
		fprintf(stderr, "LEXICAL ERROR (line %d): ID size exceeded (Maximum %d characters long) -> \%s\n", yylineno, (int)MAX_ID, id);
        system("PAUSE");
        exit(-1);
    }
}

void lexError(char* message) {
	fprintf(stderr, "LEXICAL ERROR (line %d): %s -> \%s\n", yylineno, message, yytext);
	system("PAUSE");
	exit(-1);
}

/*
int main(int argc,char *argv[]) {
	if ((yyin = fopen(argv[1], "rt")) != NULL) {
		yylex();
	}
	else {
		printf("Unable to open file: %s\n", argv[1]);
	}
	fclose(yyin);
	return 0;
}
*/