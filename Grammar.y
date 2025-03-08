%code requires {

#include <iostream>
#include <vector>
#include <string>
#include "Types.hpp"
#include "Ast.hpp"

struct YYLTYPE;

#include "Module.hpp"
extern Module *g_module;

}


%code provides {

void yyerror(YYLTYPE *locp, const char *s);
int yylex(YYSTYPE *, YYLTYPE*);
}

%define parse.error detailed
%define parse.lac full
%locations
%define api.pure full

%union {
    Number number;
    NumberType number_type;
    Operator operator_type;
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
    Program *ast_program;
    std::string *str;
}

%token BREAK CASE DEFAULT ELSE IF INPUT OUTPUT SWITCH WHILE OR AND NOT;
%token <number_type> INT FLOAT;
%token <operator_type> RELOP ADDOP MULOP; 
%token <number_type> CAST;
%token <str> ID;
%token <number> NUM;

%type <number_type> type;
%type <ast_declaration> idlist declaration;
%type <ast_declaration_block> declarations;
%type <ast_expression> factor expression term;
%type <ast_bool_expression> boolexpr boolfactor boolterm;
%type <ast_statement> stmt assignment_stmt input_stmt output_stmt if_stmt while_stmt break_stmt switch_stmt  stmt_block; 
%type <ast_switch_statement> caselist;
%type <ast_statement_list> stmtlist;
%type <ast_program> program;

%%

program: 
        declarations stmt_block
        {
            $$ = new Program();
            $$->set_line_number(@$.first_line);
            $$->set_declaration_block($1);
            $$->set_program_statements($2);
            g_module->set_ast_root($$);
        }
        ;

declarations:
        %empty { $$ = new DeclarationBlock(); }
        | declarations declaration { $1->add_declaration($2); $$ = $1; }
        ;

declaration:
        idlist ':' type ';' { $1->assign_number_type($3); $$ = $1; }
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID { $1->add_id($3); $$ = $1; }
        | ID { $$ = new Declaration(); $$->add_id($1); $$->set_line_number(@$.first_line); }
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
                    { $$ = new AssignStatement($1, $3); $$->set_line_number(@$.first_line); }

input_stmt:         INPUT '(' ID ')' ';' 
                    { $$ = new InputStatement($3); $$->set_line_number(@$.first_line); }

output_stmt:        OUTPUT '(' expression ')' ';' 
                    { $$ = new OutputStatement($3); $$->set_line_number(@$.first_line); }

if_stmt:            IF '(' boolexpr ')' stmt ELSE stmt
                    { $$ = new IfStatement($3, $5, $7); $$->set_line_number(@$.first_line); }

while_stmt:         WHILE '(' boolexpr ')' stmt 
                    { $$ = new WhileStatement($3, $5); $$->set_line_number(@$.first_line); }

switch_stmt:        SWITCH '(' expression ')'
                    '{' caselist '}' DEFAULT ':' stmtlist '}'
                    {
                        $6->set_expression($3);
                        $6->set_default_case($10);
                        $$ = $6;
                    }

caselist:              
        %empty
        { $$ = new SwitchStatement(); $$->set_line_number(@$.first_line);  }
        | caselist CASE NUM ':' stmtlist
        { $1->add_case($3, $5); $$ = $1; }
        ;
                        
break_stmt:             BREAK ';'
                        { $$ = new BreakStatement(); $$->set_line_number(@$.first_line); }

stmt_block:             '{' stmtlist '}'
                        { $$ = $2; }

stmtlist:               
        %empty 
        { $$ = new StatementList(); $$->set_line_number(@$.first_line); }
        
        | stmtlist stmt 
        { $1->add_statement($2); $$ = $1; }
        ;
                        
boolexpr:               
        boolexpr OR boolterm { $$ = new BinaryBoolExpression($1, $3, Operator::OR); $$->set_line_number(@$.first_line); }
        | boolterm
        ;

boolterm:
        boolterm AND boolfactor { $$ = new BinaryBoolExpression($1, $3, Operator::AND); $$->set_line_number(@$.first_line); }
        | boolfactor
        ;

boolfactor:
        NOT '(' boolexpr ')' {
                $$ = new NotExpression($3);
                $$->set_line_number(@$.first_line); 
        }
        | expression RELOP expression {
                $$ = new BinaryBoolExpression($1, $3, $2);
                $$->set_line_number(@$.first_line); 
        }
        ;

expression:
        expression ADDOP term { $$ = new BinaryExpression($1, $3, $2); $$->set_line_number(@$.first_line);  }
        | term { $$ = $1; }
        ;

term:
        term MULOP factor { $$ = new BinaryExpression($1, $3, $2); $$->set_line_number(@$.first_line); }
        | factor { $$ = $1; }
        ;

factor:
        '(' expression ')' { $$ = $2; }
        | CAST '(' expression ')' { $$ = new CastExpression($1, $3); $$->set_line_number(@$.first_line); }
        | ID { $$ = new IdExpression($1); $$->set_line_number(@$.first_line); }
        | NUM { $$ = new NumberExpression($1); $$->set_line_number(@$.first_line); }
        ;

%%

void yyerror(YYLTYPE *locp, const char *s)
{
    g_module->set_current_position(locp->first_line);
    g_module->emit_error(s);
    g_module->set_status(ModuleStatus::SyntaxError);
}