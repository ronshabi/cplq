//===-- Ast.cpp -----------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#include "Ast.hpp"

#include "CodeGenerator.hpp"
#include "SemanticAnalyzer.hpp"
#include "DebugStream.hpp"

void
AstNode::Dump(unsigned indent) const
{
    for (unsigned i = 0; i < indent; ++i)
    {
        std::cout << ' ';
    }
}

void
DeclarationBlock::AddDeclaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

void
DeclarationBlock::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "DeclBlock<" << m_declarations.size() << ">\n";
    for (auto const &decl : m_declarations)
    {
        decl->Dump(indent + 4);
    }
}

void
DeclarationBlock::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
DeclarationBlock::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
BinaryBoolExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "BinaryBoolExpression<" << OperatorToString(m_op) << ">\n";
    m_expr1->Dump(indent + 4);
    m_expr2->Dump(indent + 4);
}

void
BinaryBoolExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
BinaryBoolExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
Declaration::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Decl<" << NumberTypeToString(m_type) << "> "
              << VectorToString(m_ids) << "\n";
}

void
Declaration::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    UNUSED(sema);
}

void
Declaration::AcceptCodeGenerator(CodeGenerator &cg)
{
    UNUSED(cg);
}

void
NotExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "NotExp\n";
    m_expr->Dump(indent + 4);
}

void
NotExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
NotExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
NumberExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    if (m_number.type == NumberType::FLOAT)
    {
        std::cout << "Number<Float>(" << m_number.d << ")\n";
    }
    else
    {
        std::cout << "Number<Int>(" << m_number.i << ")\n";
    }
}

void
NumberExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    UNUSED(sema);
}

void
NumberExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
CastExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Cast<" << NumberTypeToString(m_cast) << ">\n";
    m_expr->Dump(indent + 4);
}

void
CastExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
CastExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
IdExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Id<" << *m_id << ">\n";
}

void
IdExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
IdExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

NumberType
IdExpression::GetType(SymbolTable const &symbol_table) const
{
    auto const sym = symbol_table.Get(*m_id);
    if (sym.has_value())
    {
        return sym->value.type;
    }
    else
    {
        return NumberType::INT; // Default value. If the number has no value
                                // It will be caught in the semantic Analyzer
                                // before this
    }
}

void
BinaryExpression::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "BinaryExpression<" << OperatorToString(m_op) << ">\n";
    m_lhsExpr->Dump(indent + 4);
    m_rhsExpr->Dump(indent + 4);
}

void
BinaryExpression::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
BinaryExpression::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

NumberType
BinaryExpression::GetType(SymbolTable const &symbol_table) const
{
    auto const lhsExprType = m_lhsExpr->GetType(symbol_table);
    auto const rhsExprType = m_rhsExpr->GetType(symbol_table);
    if (lhsExprType == NumberType::FLOAT || rhsExprType == NumberType::FLOAT)
    {
        return NumberType::FLOAT;
    }

    return NumberType::INT;
}

void
AssignStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Assign<" << *m_id << ">\n";
    m_expr->Dump(indent + 4);
}

void
AssignStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
AssignStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
OutputStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Output\n";
    m_expr->Dump(indent + 4);
}

void
OutputStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
OutputStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
InputStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Input<" << *m_id << ">\n";
}

void
InputStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
InputStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
IfStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "If\n";
    m_conditionExpr->Dump(indent + 4);
    m_thenBranch->Dump(indent + 4);
    m_elseBranch->Dump(indent + 4);
}

void
IfStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
IfStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
WhileStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "While\n";
    m_conditionExpr->Dump(indent + 4);
    m_bodyStatement->Dump(indent + 4);
}

void
WhileStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
WhileStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
BreakStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Break\n";
}

void
BreakStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    // nop
    UNUSED(sema);
}

void
BreakStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
StatementList::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "StatementList\n";
    for (auto const &stmt : m_statements)
    {
        stmt->Dump(indent + 4);
    }
}

void
StatementList::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
StatementList::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
SwitchStatement::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    std::cout << "Switch\n";
    m_expr->Dump(indent + 4);
    for (auto const &casePair : m_cases)
    {
        NumberExpression x(casePair.first);
        x.Dump(indent + 4);
        (casePair.second)->Dump(indent + 8);
    }

    AstNode::Dump(indent + 4);
    std::cout << "Default\n";
    m_defaultCase->Dump(indent + 8);
}

void
SwitchStatement::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
SwitchStatement::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}

void
Program::Dump(unsigned indent) const
{
    AstNode::Dump(indent);
    m_declarationBlock->Dump(0);
    m_programStatements->Dump(0);
}

void
Program::AcceptSemanticAnalyzer(SemanticAnalyzer &sema)
{
    sema.Analyze(this);
}

void
Program::AcceptCodeGenerator(CodeGenerator &cg)
{
    cg.Codegen(this);
}
