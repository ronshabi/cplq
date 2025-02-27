%code requires {

#include <stdio.h>
#include <vector>
#include <string>
#include "cpl_types.hxx"

void yyerror(const char *s);
int yylex(void);

}

%union {
    cpl::Number number;
    cpl::NumberType number_type;
    cpl::OperatorType operator_type;
    char *str;
    std::vector<std::string> *vec;
}

%token BREAK CASE DEFAULT ELSE IF INPUT OUTPUT SWITCH WHILE OR AND NOT;
%token <number_type> INT FLOAT;
%token <operator_type> RELOP ADDOP MULOP; 
%token <number_type> CAST;
%token <str> ID;
%token <number> NUM;

%type <number_type> type;

%%

program: 
        declarations stmt_block
        ;

declarations:
        %empty
        | declarations declaration
        ;

declaration:
        idlist ':' type ';' {
                printf("DECL type is %d\n", $3);
        }
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID {
                printf("idlist '%s'\n", $3);
        }
        | ID {
                printf("new ID: '%s'\n", $1);
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

%%

void yyerror(const char *s)
{
    printf("BISON ERROR: '%s'\n", s);
}