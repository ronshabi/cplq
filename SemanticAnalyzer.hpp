#pragma once

#include "Ast.hpp"
#include "SymbolTable.hpp"

class Module;

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(Program *ast, Module &mod)
        : m_module(mod)
        , m_ast(ast)
        , m_symbol_table()
    {
    }

    SymbolTable &get_symbol_table()
    {
        return m_symbol_table;
    }

    void analyze(Program *node);
    void analyze(DeclarationBlock *db);

private:
    Module     &m_module;
    Program    *m_ast;
    SymbolTable m_symbol_table;
};