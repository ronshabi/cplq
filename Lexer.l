%{
#include "Grammar.hpp"

#define YY_USER_ACTION yylloc->first_line = yylineno;
%}

letter  [a-zA-Z]
digit   [0-9]

%x COMMENT_MODE


%option noyywrap
%option yylineno
%option bison-locations
%option bison-bridge
%%

"break"         return BREAK;
"case"          return CASE;
"default"       return DEFAULT;
"else"          return ELSE;
"float"         { yylval->numberType = NumberType::FLOAT; return FLOAT; }
"int"           { yylval->numberType = NumberType::INT; return INT; }
"if"            return IF;
"input"         return INPUT;
"output"        return OUTPUT;
"switch"        return SWITCH;
"while"         return WHILE;
"=="            { yylval->operatorType = Operator::EQ;     return RELOP; };
"!="            { yylval->operatorType = Operator::NEQ;    return RELOP; };
"<"             { yylval->operatorType = Operator::LT;     return RELOP; };
">"             { yylval->operatorType = Operator::GT;     return RELOP; };
"<="            { yylval->operatorType = Operator::LTE;    return RELOP; };
">="            { yylval->operatorType = Operator::GTE;    return RELOP; };
"+"             { yylval->operatorType = Operator::PLUS;   return ADDOP; };
"-"             { yylval->operatorType = Operator::MINUS;  return ADDOP; };
"*"             { yylval->operatorType = Operator::MUL;    return MULOP; };
"/"             { yylval->operatorType = Operator::DIV;    return MULOP; };
"||"            { return OR; }
"&&"            { return AND; }
"!"             { return NOT; }
"cast<int>"     { yylval->numberType = NumberType::INT;   return CAST; }
"cast<float>"   { yylval->numberType = NumberType::FLOAT; return CAST; }

        /* Taken from the Flex Manual: "How can I match C-style comments?" */

<INITIAL>{
        "/*" BEGIN(COMMENT_MODE);
}

<COMMENT_MODE>{
        "*/" {BEGIN(INITIAL);}
        [^*\n]+ ;
        "*" ;
        \n ;

}

{letter}({letter}|{digit})*    {
        yylval->str = new std::string(yytext);
        return ID;
}

{digit}+            {
        yylval->number.type = NumberType::INT;
        yylval->number.i = atoi(yytext);
        yylval->number.hasValue = true;
        return NUM;
}

{digit}+\.{digit}*  {
        yylval->number.type = NumberType::FLOAT;
        yylval->number.d = atof(yytext);
        yylval->number.hasValue = true;
        return NUM;
}


[[:space:]]         ;
.                       { return *yytext; }

%%
