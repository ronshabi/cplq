%code requires {

#include <iostream>
#include <vector>
#include <string>
#include "Types.hpp"
#include "Ast.hpp"

void yyerror(const char *s);
int yylex(void);

}

%union {
    Number number;
    NumberType number_type;
    Operator operator_type;
    char *str;
    Declaration *ast_declaration;
    DeclarationBlock *ast_declaration_block;
    Expression *ast_expression;
    BoolExpression *ast_bool_expression;
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
%type <ast_expression> factor expression term;
%type <ast_bool_expression> boolexpr boolfactor boolterm;
%%

program: 
        declarations stmt_block {
            $1->print_repr();
        }
        ;

declarations:
        %empty {
            $$ = new DeclarationBlock();
        }
        | declarations declaration {
            $1->add_declaration($2);
            $$ = $1;
        }
        ;

declaration:
        idlist ':' type ';' {
            $$ = new Declaration($1, $3);
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
        boolexpr OR boolterm { $$ = new BinaryBoolExpression($1, $3, Operator::OR); }
        | boolterm
        ;

boolterm:
        boolterm AND boolfactor { $$ = new BinaryBoolExpression($1, $3, Operator::AND); }
        | boolfactor
        ;

boolfactor:
        NOT '(' boolexpr ')' {
                $$ = new NotExpression($3);
        }
        | expression RELOP expression {
                $$ = new BinaryBoolExpression($1, $3, $2);
        }
        ;

expression:
        expression ADDOP term { $$ = new BinaryExpression($1, $3, $2); }
        | term { $$ = $1; }
        ;

term:
        term MULOP factor { $$ = new BinaryExpression($1, $3, $2); }
        | factor { $$ = $1; }
        ;

factor:
        '(' expression ')' { $$ = $2; }
        | CAST '(' expression ')' { $$ = new CastExpression($1, $3); }
        | ID { $$ = new IdExpression($1); }
        | NUM { $$ = new NumberExpression($1); }
        ;

%%

void yyerror(const char *s)
{
    printf("BISON ERROR: '%s'\n", s);
}