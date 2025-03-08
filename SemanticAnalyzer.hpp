#pragma once

#include "Ast.hpp"
#include "SymbolTable.hpp"

class Module;

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(Module &mod)
        : m_module(mod)
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
    bool        m_allow_break_stmts = false;
    Module     &m_module;
};