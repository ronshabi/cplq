%code requires {

#include <iostream>
#include <vector>
#include <string>
#include "types.hxx"
#include "ast.hxx"

void yyerror(const char *s);
int yylex(void);

using namespace cpl;

}

%union {
    Number number;
    NumberType number_type;
    Operator operator_type;
    char *str;
    ast::Declaration *ast_declaration;
    ast::DeclarationBlock *ast_declaration_block;
    std::vector<std::string> *decl_names;
}

%token BREAK CASE DEFAULT ELSE IF INPUT OUTPUT SWITCH WHILE OR AND NOT;
%token <number_type> INT FLOAT;
%token <operator_type> RELOP ADDOP MULOP; 
%token <number_type> CAST;
%token <str> ID;
%token <number> NUM;

%type <number_type> type;
%type <decl_names> idlist;
%type <ast_declaration> declaration;
%type <ast_declaration_block> declarations;
%%

program: 
        declarations stmt_block {
            $1->print_repr();
        }
        ;

declarations:
        %empty {
            $$ = new ast::DeclarationBlock();
        }
        | declarations declaration {
            $1->add_declaration($2);
            $$ = $1;
        }
        ;

declaration:
        idlist ':' type ';' {
            $$ = new ast::Declaration($1, $3);
            $$->print_repr();
        }
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID { 
            $$->push_back($3);
        }
        | ID { $$ = new std::vector<std::string>(); $$->push_back($1); }
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