/* Keywords */
%token T_BREAK
%token T_CASE
%token T_DEFAULT
%token T_ELSE
%token T_FLOAT
%token T_IF
%token T_INPUT
%token T_INT
%token T_OUTPUT
%token T_SWITCH
%token T_WHILE

/* Ops */
%token T_RELOP
%token T_ADDOP
%token T_MULOP
%token T_OR
%token T_AND
%token T_NOT
%token T_CAST

/* Misc */
%token T_ID
%token T_NUM

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

type:   T_INT | T_FLOAT;

idlist:
        idlist ',' T_ID
        | T_ID
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

assignment_stmt:        T_ID '=' expression ';';
input_stmt:             T_INPUT '(' T_ID ')' ';';
output_stmt:            T_OUTPUT '(' expression ')' ';';
if_stmt:                T_IF '(' boolexpr ')' stmt T_ELSE stmt;
while_stmt:             T_WHILE '(' boolexpr ')' stmt;
switch_stmt:            T_SWITCH '(' expression ')'
                        '{' caselist '}' T_DEFAULT ':' stmtlist '}';

caselist:              
        %empty
        | caselist T_CASE T_NUM ':' stmtlist
        ;
                        
break_stmt:             T_BREAK ';';
stmt_block:             '{' stmtlist '}';

stmtlist:               
        %empty
        | stmtlist stmt
        ;
                        
boolexpr:               
        boolexpr T_OR boolterm
        | boolterm
        ;

boolterm:
        boolterm T_AND boolfactor
        | boolfactor
        ;

boolfactor:
        T_NOT '(' boolexpr ')'
        | expression T_RELOP expression
        ;

expression:
        expression T_ADDOP term
        | term
        ;

term:
        term T_MULOP factor
        | factor
        ;

factor:
        '(' expression ')'
        | T_CAST '(' expression ')'
        | T_ID
        | T_NUM
        ;