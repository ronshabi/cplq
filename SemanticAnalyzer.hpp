#pragma once

#include "SymbolTable.hpp"
#include "Ast.hpp"

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(Program *ast) : m_ast(ast), m_symbol_table() {}

    void analyze(Program *node) {
        printf("prog\n");
    }

    void analyze(DeclarationBlock *db) {
        printf("declblock\n");
    }

private:
    Program *m_ast;
    SymbolTable m_symbol_table;
};