#include <iostream>

#include "Ast.hpp"

extern void  yyparse();
extern FILE *yyin;

Program *g_program = nullptr;

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " [source-file.ou]\n";
        exit(EXIT_FAILURE);
    }

    yyin = fopen(argv[1], "r");

    // TODO: format nicely
    if (yyin == nullptr)
    {
        std::cerr << "file not found\n";
        exit(EXIT_FAILURE);
    }

    yyparse();

    g_program->dump(0);

    return 0;
}