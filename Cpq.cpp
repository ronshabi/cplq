#include <iostream>

#include "Module.hpp"

extern FILE *yyin;

Module *g_module;

void
usage(int exit_code = 1)
{
    std::cerr << "cplq compiler\nusage: ./cplq <source-file.ou> [command]\n\n";
    std::cerr << "available commands:\n";
    std::cerr << "    --dump-ast           Dumps the AST to stdout\n";
    exit(exit_code);
}

int
main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        usage();
    }

    Module main_module(argv[1]);

    g_module = &main_module;

    main_module.Parse();

    if (main_module.IsOK())
    {
        // main_module.dump_ast();
        main_module.SemanticAnalyze();
        // main_module.dump_symbol_table();
        main_module.CodeGen();
    }

    return main_module.GetErrorCode();
}