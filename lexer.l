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
"=="            { yylval.operator_type = cpl::Operator::EQ;     return RELOP; };
"!="            { yylval.operator_type = cpl::Operator::NEQ;    return RELOP; };
"<"             { yylval.operator_type = cpl::Operator::LT;     return RELOP; };
">"             { yylval.operator_type = cpl::Operator::GT;     return RELOP; };
"<="            { yylval.operator_type = cpl::Operator::LTE;    return RELOP; };
">="            { yylval.operator_type = cpl::Operator::GTE;    return RELOP; };
"+"             { yylval.operator_type = cpl::Operator::PLUS;   return ADDOP; };
"-"             { yylval.operator_type = cpl::Operator::MINUS;  return ADDOP; };
"*"             { yylval.operator_type = cpl::Operator::MUL;    return MULOP; };
"/"             { yylval.operator_type = cpl::Operator::DIV;    return MULOP; };
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

