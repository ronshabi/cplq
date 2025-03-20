//===-- SemanticAnalyzer.cpp-----------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#include "SemanticAnalyzer.hpp"

#include "Module.hpp"
#include <unordered_set>

SymbolTable &
SemanticAnalyzer::GetSymbolTable()
{
    return m_module.GetSymbolTable();
}

void
SemanticAnalyzer::Analyze(Program *node)
{
    node->GetDeclarationBlock()->AcceptSemanticAnalyzer(*this);
    node->GetStatemenets()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(DeclarationBlock *node)
{
    auto const decls = node->GetDeclarations();
    for (auto const decl : decls)
    {
        m_module.SetCurrentPosition(decl->GetLineNumber());

        auto const declIds = decl->GetIDs();
        for (auto const id : declIds)
        {
            auto const idOptional = GetSymbolTable().Get(*id);

            // ID already exists in the symbol table. This is an Error.
            if (idOptional.has_value())
            {
                // Give detailed error based on its type
                m_module.EmitError("repeated declaration of symbol '%s'",
                                    id->c_str());
            }
            // ID doesn't exist in the symbol table, so add it.
            else
            {
                GetSymbolTable().Add(
                    *id, Symbol(decl->GetType(), decl->GetLineNumber()));
            }
        }
    }
}

void
SemanticAnalyzer::Analyze(StatementList *node)
{
    for (auto const &stmt : node->GetStatements())
    {
        stmt->AcceptSemanticAnalyzer(*this);

        if (!m_allowBreakStmts && stmt->IsBreakStatement())
        {
            m_module.SetCurrentPosition(stmt->GetLineNumber());
            m_module.EmitError(
                "break statements can appear only in switch cases");
        }
    }
}

void
SemanticAnalyzer::Analyze(AssignStatement *node)
{
    // Check that the ID is in the symbol table
    auto const idSym = GetSymbolTable().Get(*node->GetID());
    m_module.SetCurrentPosition(node->GetLineNumber());

    if (!idSym.has_value())
    {
        m_module.EmitError("unknown indentifier '%s' in assign statement",
                            node->GetID()->c_str());
    }

    // Check the expression
    node->GetExpr()->AcceptSemanticAnalyzer(*this);

    // Check that the expression is not a boolean expression
    if (node->GetExpr()->IsBoolExpr())
    {
        m_module.EmitError("a boolean expression cannot be an asignee");
        return;
    }

    // Typechecker: Check the type of the expression, if it's a float, and we're
    // an int, deny
    if (node->GetExpr()->GetType(GetSymbolTable()) ==
            NumberType::FLOAT &&
        idSym->value.type == NumberType::INT)
    {
        m_module.EmitError(
            "type mismatch: can't assign a float to an int variable");
        return;
    }
}

void
SemanticAnalyzer::Analyze(InputStatement *node)
{
    // Check the indentifier is in the symbol table
    auto const id  = node->GetID();
    auto const sym = GetSymbolTable().Get(*id);
    if (!sym.has_value())
    {
        m_module.SetCurrentPosition(node->GetLineNumber());
        m_module.EmitError("unknown indentifier '%s' in input function",
                            id->c_str());
    }
}

void
SemanticAnalyzer::Analyze(OutputStatement *node)
{
    node->GetExpr()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(IfStatement *node)
{
    node->GetConditionExpr()->AcceptSemanticAnalyzer(*this);
    node->GetThenBranch()->AcceptSemanticAnalyzer(*this);
    node->GetElseBranch()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(BinaryExpression *node)
{
    node->GetLhsExpr()->AcceptSemanticAnalyzer(*this);
    node->GetRhsExpr()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(IdExpression *node)
{
    // Check the ID is in the symbol table
    auto const sym = GetSymbolTable().Get(*node->GetID());
    if (!sym.has_value())
    {
        m_module.SetCurrentPosition(node->GetLineNumber());
        m_module.EmitError("unknown variable '%s'", node->GetID()->c_str());
    }
}

void
SemanticAnalyzer::Analyze(SwitchStatement *node)
{

    // Check expression
    m_module.SetCurrentPosition(node->GetLineNumber());
    node->GetExpr()->AcceptSemanticAnalyzer(*this);
    auto const switchedExprType =
        node->GetExpr()->GetType(GetSymbolTable());

    if (switchedExprType != NumberType::INT)
    {
        m_module.EmitError(
            "type mismatch: switched expression must be an integer");
    }

    // Check no ambiguous cases
    auto const              cases = node->GetCases();
    std::unordered_set<int> allCaseNumbers;

    for (auto const &[caseNum, caseStmt] : cases)
    {
        // Check case didn't occur before. If it did, emit error
        m_module.SetCurrentPosition(caseStmt->GetLineNumber());
        if (caseNum.type != NumberType::INT)
        {
            m_module.EmitError(
                "type mismatch: case number '%.5g' must be an integer",
                caseNum.d);
        }

        if (allCaseNumbers.contains(caseNum.i))
        {
            m_module.EmitError(
                "switch contains ambiguous cases");
        }

        allCaseNumbers.insert(caseNum.i);


        // We have been called by a switch/while, so breaks here are OK
        bool gotCalledBySwitch = m_allowBreakStmts;
        

        m_allowBreakStmts = true; // Allow break statements.
        caseStmt->AcceptSemanticAnalyzer(*this);

        if (!gotCalledBySwitch) {
            m_allowBreakStmts = false; // Disallow break statements.
        }
    }

    // Check that the default statement is okay
    bool gotCalledBySwitch = m_allowBreakStmts;
    m_allowBreakStmts = true; // Allow break statements.
    node->GetDefaultCase()->AcceptSemanticAnalyzer(*this);
    if (!gotCalledBySwitch) {
        m_allowBreakStmts = false; // Disallow break statements.
    }
}

void
SemanticAnalyzer::Analyze(WhileStatement *node)
{
    bool gotCalledByBreakAllowedStmt = m_allowBreakStmts;

    m_allowBreakStmts = true;
    node->GetConditionExpr()->AcceptSemanticAnalyzer(*this);
    node->GetBodyStatement()->AcceptSemanticAnalyzer(*this);

    if (!gotCalledByBreakAllowedStmt) {
        m_allowBreakStmts = false;
    }
}

void
SemanticAnalyzer::Analyze(CastExpression *node)
{
    node->GetExpr()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(NotExpression *node)
{
    node->GetExpr()->AcceptSemanticAnalyzer(*this);
}

void
SemanticAnalyzer::Analyze(BinaryBoolExpression *node)
{
    node->GetLhsExpr()->AcceptSemanticAnalyzer(*this);
    node->GetRhsExpr()->AcceptSemanticAnalyzer(*this);
}
