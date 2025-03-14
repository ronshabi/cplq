%{
#include "Grammar.hpp"

#define YY_USER_ACTION yylloc->first_line = yylineno;
%}

letter  [a-zA-Z]
digit   [0-9]


%option noyywrap
%option yylineno
%option bison-locations
%option bison-bridge
%%

"break"         return BREAK;
"case"          return CASE;
"default"       return DEFAULT;
"else"          return ELSE;
"float"         { yylval->number_type = NumberType::FLOAT; return FLOAT; }
"int"           { yylval->number_type = NumberType::INT; return INT; }
"if"            return IF;
"input"         return INPUT;
"output"        return OUTPUT;
"switch"        return SWITCH;
"while"         return WHILE;
"=="            { yylval->operator_type = Operator::EQ;     return RELOP; };
"!="            { yylval->operator_type = Operator::NEQ;    return RELOP; };
"<"             { yylval->operator_type = Operator::LT;     return RELOP; };
">"             { yylval->operator_type = Operator::GT;     return RELOP; };
"<="            { yylval->operator_type = Operator::LTE;    return RELOP; };
">="            { yylval->operator_type = Operator::GTE;    return RELOP; };
"+"             { yylval->operator_type = Operator::PLUS;   return ADDOP; };
"-"             { yylval->operator_type = Operator::MINUS;  return ADDOP; };
"*"             { yylval->operator_type = Operator::MUL;    return MULOP; };
"/"             { yylval->operator_type = Operator::DIV;    return MULOP; };
"||"            { return OR; }
"&&"            { return AND; }
"!"             { return NOT; }
"cast<int>"     { yylval->number_type = NumberType::INT;   return CAST; }
"cast<float>"   { yylval->number_type = NumberType::FLOAT; return CAST; }

"/*"(.|\n)+"*/" ;

{letter}({letter}|{digit})*    {
        yylval->str = new std::string(yytext);
        return ID;
}

{digit}+            {
        yylval->number.type = NumberType::INT;
        yylval->number.i = atoi(yytext);
        yylval->number.has_value = true;
        return NUM;
}

{digit}+\.{digit}*  {
        yylval->number.type = NumberType::FLOAT;
        yylval->number.d = atof(yytext);
        yylval->number.has_value = true;
        return NUM;
}


[[:space:]]         ;
.                       { return *yytext; }

%%
