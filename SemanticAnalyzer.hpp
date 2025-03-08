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
    {
    }

    SymbolTable &get_symbol_table();

    void analyze(Program *node);
    void analyze(DeclarationBlock *node);
    void analyze(StatementList *node);
    void analyze(AssignStatement *node);
    void analyze(InputStatement *node);
    void analyze(OutputStatement *node);
    void analyze(IfStatement *node);
    void analyze(SwitchStatement *node);
    void analyze(WhileStatement *node);
    void analyze(CastExpression *node);
    void analyze(NotExpression *node);
    void analyze(BinaryBoolExpression *node);
    void analyze(BinaryExpression *node);
    void analyze(IdExpression *node);

private:
    Module     &m_module;
    Program    *m_ast;
};