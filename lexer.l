%{
#include "grammar.hxx"
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
"float"         { yylval.number_type = cpl::NumberType::FLOAT; return FLOAT; }
"int"           { yylval.number_type = cpl::NumberType::INT; return INT; }
"if"            return IF;
"input"         return INPUT;
"output"        return OUTPUT;
"switch"        return SWITCH;
"while"         return WHILE;
"=="            { yylval.operator_type = cpl::OperatorType::EQ;     return RELOP; };
"!="            { yylval.operator_type = cpl::OperatorType::NEQ;    return RELOP; };
"<"             { yylval.operator_type = cpl::OperatorType::LT;     return RELOP; };
">"             { yylval.operator_type = cpl::OperatorType::GT;     return RELOP; };
"<="            { yylval.operator_type = cpl::OperatorType::LTE;    return RELOP; };
">="            { yylval.operator_type = cpl::OperatorType::GTE;    return RELOP; };
"+"             { yylval.operator_type = cpl::OperatorType::PLUS;   return ADDOP; };
"-"             { yylval.operator_type = cpl::OperatorType::MINUS;  return ADDOP; };
"*"             { yylval.operator_type = cpl::OperatorType::MUL;    return MULOP; };
"/"             { yylval.operator_type = cpl::OperatorType::DIV;    return MULOP; };
"||"            { return OR; }
"&&"            { return AND; }
"!"             { return NOT; }
"cast<int>"     { yylval.number_type = cpl::NumberType::INT;   return CAST; }
"cast<float>"   { yylval.number_type = cpl::NumberType::FLOAT; return CAST; }

"/*"(.|\n)+"*/" ;

{letter}({letter}|{digit})*    {
        yylval.str = yytext;
        return ID;
}

{digit}+            {
        yylval.number.type = cpl::NumberType::INT;
        yylval.number.i = atoi(yytext);
        return NUM;
}

{digit}+\.{digit}*  {
        yylval.number.type = cpl::NumberType::FLOAT;
        yylval.number.d = atof(yytext);
        return NUM;
}


[[:space:]]         ;
.                       { return *yytext; }
%%

