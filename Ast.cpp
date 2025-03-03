#include "Ast.hpp"

void DeclarationBlock::add_declaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

void DeclarationBlock::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "DeclBlock<" << m_declarations.size() << ">\n";
    for (const auto& decl : m_declarations) {
        decl->dump(indent + 4);
    }
}

bool BinaryBoolExpression::evaluate() const
{
    // TODO
    return false;
}

void BinaryBoolExpression::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "BinaryBoolExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void Declaration::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Decl<" << number_type_to_string(m_type) << "> " << vector_to_string(m_ids) << "\n";
}

void NotExpression::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "NotExp\n";
    m_exp->dump(indent + 4);
}

void NumberExpression::dump(unsigned indent) const {
    Node::dump(indent);
    if (m_number.type == NumberType::FLOAT) {
        std::cout << "Number<Float>(" << m_number.d << ")\n";
    } else {
        std::cout << "Number<Int>(" << m_number.i << ")\n";
    }
}

void CastExpression::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Cast<" << number_type_to_string(m_cast) << ">\n";
    m_expression->dump(indent+4);
}

void IdExpression::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Id<" << *m_id << ">\n";
}

void BinaryExpression::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "BinaryExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void AssignStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Assign<" << *m_id << ">\n";
    m_exp->dump(indent+4);
}

void OutputStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Output\n";
    m_exp->dump(indent+4);
}

void InputStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Input<" << *m_id << ">\n";
}

void IfStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "If\n";
    m_condition_exp->dump(indent+4);
    m_then_branch->dump(indent+4);
    m_else_branch->dump(indent+4);
}

void WhileStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "While\n";
    m_condition_exp->dump(indent+4);
    m_body_statement->dump(indent+4);
}

void BreakStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Break\n";
}

void StatementList::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "StatementList\n";
    for (auto const& stmt : m_statements) {
        stmt->dump(indent+4);
    }
}

void SwitchStatement::dump(unsigned indent) const {
    Node::dump(indent);
    std::cout << "Switch\n";
    m_exp->dump(indent+4);
    for (auto const &case_pair: m_cases) {
        NumberExpression x(case_pair.first);
        x.dump(indent+4);
        (case_pair.second)->dump(indent+8);
    }

    Node::dump(indent+4);
    std::cout << "Default\n";
    m_default_case->dump(indent+8);
}

void Program::dump(unsigned indent) const {
    Node::dump(indent);
    m_declaration_block->dump(0);
    m_program_statements->dump(0);
}
