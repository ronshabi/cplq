//===-- Cpq.cpp -----------------------------------------------------------===//
//
// This is the main entry point for the compiler.
// We do that by creating a `Module` class and passing the path of the source
// file we want to compile to it. Additionally, we assign the global pointer
// g_module to the module created in the main function here, to have Bison
// interact with it.
//
// The global module pointer will not be a dangling pointer, because its
// lifetime is exactly the lifetime of the entire program run, and Bison is not
// called before this pointer is assigned to the only module in memory, which is
// the module created here.
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#include <iostream>

#include "Module.hpp"

extern FILE *yyin;

Module *g_module;

void
usage(int exit_code = 1)
{
    std::cerr << "cplq compiler\nusage: ./cplq <source-file.ou> [command]\n";
    exit(exit_code);
}

int
main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        usage();
    }

    Module mainModule(argv[1]);

    g_module = &mainModule;

    mainModule.Parse();

    if (mainModule.IsOK())
    {
        // mainModule.dump_ast();
        mainModule.SemanticAnalyze();
        // mainModule.dump_symbol_table();
        mainModule.CodeGen();
    }

    return mainModule.GetErrorCode();
}