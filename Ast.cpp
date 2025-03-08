#include "Ast.hpp"

#include "SemanticAnalyzer.hpp"

void AstNode::dump(unsigned indent) const
{
    for (unsigned i = 0; i < indent; ++i)
    {
        std::cout << ' ';
    }
}

void DeclarationBlock::add_declaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

void DeclarationBlock::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "DeclBlock<" << m_declarations.size() << ">\n";
    for (const auto &decl : m_declarations)
    {
        decl->dump(indent + 4);
    }
}

void DeclarationBlock::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

bool BinaryBoolExpression::evaluate() const
{
    // TODO
    return false;
}

void BinaryBoolExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "BinaryBoolExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void BinaryBoolExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void Declaration::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Decl<" << number_type_to_string(m_type) << "> "
              << vector_to_string(m_ids) << "\n";
}

void Declaration::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void NotExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "NotExp\n";
    m_exp->dump(indent + 4);
}

void NotExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void NumberExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    if (m_number.type == NumberType::FLOAT)
    {
        std::cout << "Number<Float>(" << m_number.d << ")\n";
    }
    else
    {
        std::cout << "Number<Int>(" << m_number.i << ")\n";
    }
}

void NumberExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void CastExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Cast<" << number_type_to_string(m_cast) << ">\n";
    m_expression->dump(indent + 4);
}

void CastExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void IdExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Id<" << *m_id << ">\n";
}

void IdExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void BinaryExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "BinaryExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void BinaryExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void AssignStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Assign<" << *m_id << ">\n";
    m_exp->dump(indent + 4);
}

void AssignStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void OutputStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Output\n";
    m_exp->dump(indent + 4);
}

void OutputStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void InputStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Input<" << *m_id << ">\n";
}

void InputStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void IfStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "If\n";
    m_condition_exp->dump(indent + 4);
    m_then_branch->dump(indent + 4);
    m_else_branch->dump(indent + 4);
}

void IfStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void WhileStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "While\n";
    m_condition_exp->dump(indent + 4);
    m_body_statement->dump(indent + 4);
}

void WhileStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void BreakStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Break\n";
}

void BreakStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void StatementList::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "StatementList\n";
    for (auto const &stmt : m_statements)
    {
        stmt->dump(indent + 4);
    }
}

void StatementList::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void SwitchStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Switch\n";
    m_exp->dump(indent + 4);
    for (auto const &case_pair : m_cases)
    {
        NumberExpression x(case_pair.first);
        x.dump(indent + 4);
        (case_pair.second)->dump(indent + 8);
    }

    AstNode::dump(indent + 4);
    std::cout << "Default\n";
    m_default_case->dump(indent + 8);
}

void SwitchStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

void Program::dump(unsigned indent) const
{
    AstNode::dump(indent);
    m_declaration_block->dump(0);
    m_program_statements->dump(0);
}

void Program::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}
