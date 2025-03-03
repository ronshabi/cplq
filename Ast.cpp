#include "Ast.hpp"

#include <iostream>

void Declaration::print_repr() const
{
    std::cout << "Declaration<"
              << number_type_to_string(static_cast<NumberType>(m_type))
              << "> [";
    for (std::size_t i = 0; i < m_ids.size(); ++i)
    {
        std::cout << *m_ids[i];

        if (i + 1 != m_ids.size())
        {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

void DeclarationBlock::print_repr() const
{
    std::cout << "DeclarationBlock<" << m_declarations.size() << ">:\n";
    for (auto const &decl : m_declarations)
    {
        std::cout << "  ";
        decl->print_repr();
    }
}

void DeclarationBlock::add_declaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

void BinaryExpression::print_repr() const
{
    std::cout << "BinaryExpression<" << operator_to_string(m_op) << ">\n";
    std::cout << "    ";
    m_exp1->print_repr();
    std::cout << "\n";
    std::cout << "    ";
    m_exp2->print_repr();
    std::cout << "\n";
}

void NumberExpression::print_repr() const
{
    if (m_number.type == NumberType::FLOAT)
    {
        std::cout << "Number<Float>(" << m_number.d << ")\n";
    }
    else
    {
        std::cout << "Number<Int>(" << m_number.i << ")\n";
    }
}

void CastExpression::print_repr() const
{
    if (m_cast == NumberType::FLOAT)
    {
        std::cout << "Cast<Float>:";
    }
    else
    {
        std::cout << "Cast<Int>:";
    }

    std::cout << "    ";
    m_expression->print_repr();
    std::cout << "\n";
}

void IdExpression::print_repr() const
{
    std::cout << "ID<" << *m_id << ">\n";
}

void NotExpression::print_repr() const
{
    std::cout << "NotExpression:\n";
    std::cout << "    ";
    m_exp->print_repr();
    std::cout << "\n";
}

void BinaryBoolExpression::print_repr() const
{
    std::cout << "TODO: binary bool expr\n";
}

bool BinaryBoolExpression::evaluate() const
{
    // TODO
    return false;
}

void AssignStatement::print_repr() const
{
    std::cout << "Assign<" << *m_id << ">:\n";
    std::cout << "    ";
    m_exp->print_repr();
    std::cout << "\n";
}

void InputStatement::print_repr() const
{
    std::cout << "Input<" << *m_id << ">\n";
}

void IfStatement::print_repr() const
{
    std::cout << "If:\n";
    std::cout << "    Condition: ";
    m_condition_exp->print_repr();
    std::cout << "    Then: ";
    m_then_branch->print_repr();
    std::cout << "    Else: ";
    m_else_branch->print_repr();
}
