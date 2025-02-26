%code requires {
#include "bison_types.h"    

#include <stdio.h>

void yyerror(const char *s);
int yylex(void);

}

%union {
    struct number num;
    enum operator_type op;
    enum cast_type cast;
    char* sym;
}

%token BREAK CASE DEFAULT ELSE FLOAT IF INPUT INT OUTPUT SWITCH WHILE OR AND NOT;
%token <op> RELOP ADDOP MULOP; 
%token <cast> CAST;
%token <sym> ID;
%token <num> NUM;

%%

program: 
        declarations stmt_block
        ;

declarations:
        %empty
        | declarations declaration
        ;

declaration:
        idlist ':' type ';'
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID {
                printf("idlist '%s'\n", $3);
        }
        | ID {
                printf("lone id: '%s'\n", $1);
        }
        ;

stmt:
        assignment_stmt
        | input_stmt
        | output_stmt
        | if_stmt
        | while_stmt
        | switch_stmt
        | break_stmt
        | stmt_block
        ;

assignment_stmt:        ID '=' expression ';';
input_stmt:             INPUT '(' ID ')' ';';
output_stmt:            OUTPUT '(' expression ')' ';';
if_stmt:                IF '(' boolexpr ')' stmt ELSE stmt;
while_stmt:             WHILE '(' boolexpr ')' stmt;
switch_stmt:            SWITCH '(' expression ')'
                        '{' caselist '}' DEFAULT ':' stmtlist '}';

caselist:              
        %empty
        | caselist CASE NUM ':' stmtlist
        ;
                        
break_stmt:             BREAK ';';
stmt_block:             '{' stmtlist '}';

stmtlist:               
        %empty
        | stmtlist stmt
        ;
                        
boolexpr:               
        boolexpr OR boolterm
        | boolterm
        ;

boolterm:
        boolterm AND boolfactor
        | boolfactor
        ;

boolfactor:
        NOT '(' boolexpr ')'
        | expression RELOP expression
        ;

expression:
        expression ADDOP term
        | term
        ;

term:
        term MULOP factor
        | factor
        ;

factor:
        '(' expression ')'
        | CAST '(' expression ')'
        | ID
        | NUM
        ;