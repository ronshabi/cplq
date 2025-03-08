#pragma once

#include "SymbolTable.hpp"
#include "Ast.hpp"

class Module;

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(Program *ast, Module& mod) : m_ast(ast), m_symbol_table(), m_module(mod) {}

    void analyze(Program *node);
    void analyze(DeclarationBlock *db);

private:
    Module& m_module;
    Program *m_ast;
    SymbolTable m_symbol_table;
};