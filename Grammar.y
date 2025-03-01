%code requires {

#include <iostream>
#include <vector>
#include <string>
#include "Types.hpp"
#include "Ast.hpp"

void yyerror(const char *s);
int yylex(void);

extern Program *g_program;

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
    Statement *ast_statement;
    AssignStatement *ast_assign_statement;
    OutputStatement *ast_output_statement;
    InputStatement *ast_input_statement;
    IfStatement *ast_if_statement;
    WhileStatement *ast_while_statement;
    BreakStatement *ast_break_statement;
    StatementList *ast_statement_list;
    SwitchStatement *ast_switch_statement;
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
%type <ast_statement> stmt assignment_stmt input_stmt output_stmt if_stmt while_stmt break_stmt switch_stmt  stmt_block; 
%type <ast_switch_statement> caselist;
%type <ast_statement_list> stmtlist;

%%

program: 
        declarations stmt_block
        {
            g_program = new Program();
            g_program->set_declaration_block($1);
            g_program->set_program_statements($2);
        }
        ;

declarations:
        %empty { $$ = new DeclarationBlock(); }
        | declarations declaration { $1->add_declaration($2); $$ = $1; }
        ;

declaration:
        idlist ':' type ';' { $$ = new Declaration($1, $3); }
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID { $$->push_back($3); }
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

assignment_stmt:    ID '=' expression ';' 
                    { $$ = new AssignStatement($1, $3); }

input_stmt:         INPUT '(' ID ')' ';' 
                    { $$ = new InputStatement($3); }

output_stmt:        OUTPUT '(' expression ')' ';' 
                    { $$ = new OutputStatement($3); }

if_stmt:            IF '(' boolexpr ')' stmt ELSE stmt
                    { $$ = new IfStatement($3, $5, $7); }

while_stmt:         WHILE '(' boolexpr ')' stmt 
                    { $$ = new WhileStatement($3, $5); }

switch_stmt:        SWITCH '(' expression ')'
                    '{' caselist '}' DEFAULT ':' stmtlist '}'
                    {
                        $6->set_expression($3);
                        $6->set_default_case($10);
                        $$ = $6;
                    }

caselist:              
        %empty
        { $$ = new SwitchStatement(); }
        | caselist CASE NUM ':' stmtlist
        { $1->add_case($3, $5); $$ = $1; }
        ;
                        
break_stmt:             BREAK ';'
                        { $$ = new BreakStatement(); }

stmt_block:             '{' stmtlist '}'
                        { $$ = $2; }

stmtlist:               
        %empty 
        { $$ = new StatementList(); }
        
        | stmtlist stmt 
        { $1->add_statement($2); $$ = $1; }
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