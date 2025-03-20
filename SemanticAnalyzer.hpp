#pragma once

#include "Ast.hpp"
#include "SymbolTable.hpp"

class Module;

/// \class SemanticAnalyzer
/// \brief Class responsible for semantic analysis of a module.
class SemanticAnalyzer
{
public:
    /// \brief Initializes the Analyzer with a reference to a module.
    ///
    /// \param mod The module to Analyze.
    SemanticAnalyzer(Module &mod)
        : m_module(mod)
    {
    }

    SymbolTable &GetSymbolTable();

    void Analyze(Program *node);
    void Analyze(DeclarationBlock *node);
    void Analyze(StatementList *node);
    void Analyze(AssignStatement *node);
    void Analyze(InputStatement *node);
    void Analyze(OutputStatement *node);
    void Analyze(IfStatement *node);
    void Analyze(SwitchStatement *node);
    void Analyze(WhileStatement *node);
    void Analyze(CastExpression *node);
    void Analyze(NotExpression *node);
    void Analyze(BinaryBoolExpression *node);
    void Analyze(BinaryExpression *node);
    void Analyze(IdExpression *node);

private:
    bool    m_allowBreakStmts = false;
    Module &m_module;
};