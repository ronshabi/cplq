%{
#include "parser.h"
%}

letter  [a-zA-Z]
digit   [0-9]

%option noyywrap
%option yylineno

%%

"break"         return BREAK;
"case"          return CASE;
"default"       return DEFAULT;
"else"          return ELSE;
"float"         return FLOAT;
"if"            return IF;
"input"         return INPUT;
"output"        return OUTPUT;
"switch"        return SWITCH;
"while"         return WHILE;
"=="            { yylval.op = OP_EQ;     return RELOP; };
"!="            { yylval.op = OP_NEQ;    return RELOP; };
"<"             { yylval.op = OP_LT;     return RELOP; };
">"             { yylval.op = OP_GT;     return RELOP; };
"<="            { yylval.op = OP_LTE;    return RELOP; };
">="            { yylval.op = OP_GTE;    return RELOP; };
"+"             { yylval.op = OP_PLUS;   return ADDOP; };
"-"             { yylval.op = OP_MINUS;  return ADDOP; };
"*"             { yylval.op = OP_MUL;    return MULOP; };
"/"             { yylval.op = OP_DIV;    return MULOP; };
"||"            { return OR; }
"&&"            { return AND; }
"!"             { return NOT; }
"cast<int>"     { yylval.cast = CAST_INT;   return CAST; }
"cast<float>"   { yylval.cast = CAST_FLOAT; return CAST; }

"/*"(.|\n)+"*/" ;

{letter}({letter}|{digit})*    {
        return ID;
}

{digit}+            {
        yylval.num.type = NUMBER_INT;
        yylval.num.ival = atoi(yytext);
        return NUM;
}

{digit}+\.{digit}*  {
        yylval.num.type = NUMBER_FLOAT;
        yylval.num.dval = atof(yytext);
        return NUM;
}


[[:space:]]         ;
.                       { return *yytext; }
%%

