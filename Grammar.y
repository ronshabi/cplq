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
    NumberType numberType;
    Operator operatorType;
    Declaration *ast_declaration;
    DeclarationBlock *ast_declarationBlock;
    Expression *ast_expression;
    BoolExpression *ast_boolExpression;
    Statement *ast_statement;
    AssignStatement *ast_assignStatement;
    OutputStatement *ast_outputStatement;
    InputStatement *ast_inputStatement;
    IfStatement *ast_ifStatement;
    WhileStatement *ast_whileStatement;
    BreakStatement *ast_breakStatement;
    StatementList *ast_statementList;
    SwitchStatement *ast_switchStatement;
    Program *ast_program;
    std::string *str;
}

%token BREAK "'break'";
%token CASE "'case'";
%token DEFAULT "'default'";
%token ELSE "'else'";
%token IF "'if'";
%token INPUT "'input'";
%token OUTPUT "'output'";
%token SWITCH "'switch'";
%token WHILE "'while'";
%token OR "'or'";
%token AND "'and'";
%token NOT "'not'";
%token <numberType> INT "'int'";
%token <numberType> FLOAT "'float'"
%token <operatorType> RELOP "'==', '!=', '<', '>', '<=' or '>='"; 
%token <operatorType> ADDOP "'+' or '-'"; 
%token <operatorType> MULOP "'*' or '/'"; 
%token <numberType> CAST "'cast<int>' or 'cast<float>'";
%token <str> ID "identifier";
%token <number> NUM "number";

%type <numberType> type;
%type <ast_declaration> idlist declaration;
%type <ast_declarationBlock> declarations;
%type <ast_expression> factor expression term;
%type <ast_boolExpression> boolexpr boolfactor boolterm;
%type <ast_statement> stmt assignment_stmt input_stmt output_stmt if_stmt while_stmt break_stmt switch_stmt  stmt_block; 
%type <ast_switchStatement> caselist;
%type <ast_statementList> stmtlist;
%type <ast_program> program;

/* %right '=';
%left OR;
%left AND;
%left RELOP;
%left ADDOP;
%left MULOP;
%right CAST NOT; */

%%
program: 
        declarations stmt_block
        {
            $$ = new Program();
            $$->SetLineNumber(@$.first_line);
            $$->SetDeclarationBlock($1);
            $$->SetProgramStatements($2);
            g_module->SetAstRoot($$);
        }
        ;

declarations:
        %empty { $$ = new DeclarationBlock(); }
        | declarations declaration { $1->AddDeclaration($2); $$ = $1; }
        ;

declaration:
        idlist ':' type ';' { $1->AssignNumberType($3); $$ = $1; }
        | idlist ':' error ';' { $$ = nullptr; }
        ;

type:   INT | FLOAT;

idlist:
        idlist ',' ID { $1->AddID($3); $$ = $1; }
        | ID { $$ = new Declaration(); $$->AddID($1); $$->SetLineNumber(@$.first_line); }
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
                    { $$ = new AssignStatement($1, $3); $$->SetLineNumber(@$.first_line); }
                    | ID '=' error ';' 
                    { $$ = nullptr; }

input_stmt:         INPUT '(' ID ')' ';' 
                    { $$ = new InputStatement($3); $$->SetLineNumber(@$.first_line); }

output_stmt:        OUTPUT '(' expression ')' ';' 
                    { $$ = new OutputStatement($3); $$->SetLineNumber(@$.first_line); }
                    | OUTPUT '(' error ')' ';' 
                    { $$ = nullptr; }

if_stmt:            IF '(' boolexpr ')' stmt ELSE stmt
                    { $$ = new IfStatement($3, $5, $7); $$->SetLineNumber(@$.first_line); }
                    | IF '(' error ')' stmt ELSE stmt
                    { $$ = nullptr; }

while_stmt:         WHILE '(' boolexpr ')' stmt 
                    { $$ = new WhileStatement($3, $5); $$->SetLineNumber(@$.first_line); }

switch_stmt:        SWITCH '(' expression ')'
                    '{' caselist DEFAULT ':' stmtlist '}'
                    {
                        $6->SetExpr($3);
                        $6->SetDefaultCase($9);
                        $$ = $6;
                    }

caselist:              
        %empty
        { $$ = new SwitchStatement(); $$->SetLineNumber(@$.first_line);  }
        | caselist CASE NUM ':' stmtlist
        { $1->AddCase($3, $5); $$ = $1;}
        ;
                        
break_stmt:             BREAK ';'
                        { $$ = new BreakStatement(); $$->SetLineNumber(@$.first_line); }

stmt_block:             '{' stmtlist '}'
                        { $$ = $2; }

stmtlist:               
        %empty 
        { $$ = new StatementList(); $$->SetLineNumber(@$.first_line); }
        
        | stmtlist stmt 
        { $1->AddStatement($2); $$ = $1; }

        | stmtlist error
        ;
                        
boolexpr:               
        boolexpr OR boolterm { $$ = new BinaryBoolExpression($1, $3, Operator::OR); $$->SetLineNumber(@$.first_line); }
        | boolterm
        ;

boolterm:
        boolterm AND boolfactor { $$ = new BinaryBoolExpression($1, $3, Operator::AND); $$->SetLineNumber(@$.first_line); }
        | boolfactor
        ;

boolfactor:
        NOT '(' boolexpr ')' {
                $$ = new NotExpression($3);
                $$->SetLineNumber(@$.first_line); 
        }
        | expression RELOP expression {
                $$ = new BinaryBoolExpression($1, $3, $2);
                $$->SetLineNumber(@$.first_line); 
        }
        ;

expression:
        expression ADDOP term { $$ = new BinaryExpression($1, $3, $2); $$->SetLineNumber(@$.first_line);  }
        | term { $$ = $1; }
        ;

term:
        term MULOP factor { $$ = new BinaryExpression($1, $3, $2); $$->SetLineNumber(@$.first_line); }
        | factor { $$ = $1; }
        ;

factor:
        '(' expression ')' { $$ = $2; }
        | CAST '(' expression ')' { $$ = new CastExpression($1, $3); $$->SetLineNumber(@$.first_line); }
        | ID { $$ = new IdExpression($1); $$->SetLineNumber(@$.first_line); }
        | NUM { $$ = new NumberExpression($1); $$->SetLineNumber(@$.first_line); }
        ;

%%

void yyerror(YYLTYPE *locp, const char *s)
{
    g_module->SetCurrentPosition(locp->first_line);
    g_module->EmitError(s);
    g_module->SetStatus(ModuleStatus::SyntaxError);
}