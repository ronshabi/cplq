#include <iostream>

#include "Ast.hpp"

extern void  yyparse();
extern FILE *yyin;

Program *g_program = nullptr;
bool g_dump_ast = false;

void usage(int exit_code = 1)
{
    std::cerr << "cplq compiler\nusage: ./cplq <source-file.ou> [command]\n\n";
    std::cerr << "available commands:\n";
    std::cerr << "    --dump-ast           Dumps the AST to stdout\n";
    exit(exit_code);
}

int main(int argc, const char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        usage();
    }

    std::string source_file = argv[1];

    if (argc == 3) {
        std::string command = argv[2];
        if (command == "--dump-ast") {
            g_dump_ast = true;
        } else {
            std::cerr << "unknown command\n";
            usage();
        }
    }

    yyin = fopen(argv[1], "r");

    // TODO: format nicely
    if (yyin == nullptr)
    {
        std::cerr << "file not found\n";
        exit(EXIT_FAILURE);
    }

    yyparse();

    if (g_dump_ast) {
        g_program->dump(0);
    }

    return 0;
}