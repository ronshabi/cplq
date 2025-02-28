#include "Ast.hpp"

#include <iostream>

Declaration::Declaration(std::vector<std::string> *id_vec_ptr,
                         NumberType decl_type)
    : Node(), m_ids(id_vec_ptr), m_type(decl_type)
{
}

void Declaration::print_repr() const
{
    std::cout << "Declaration<" << to_string(m_type) << "> [";
    for (std::size_t i = 0; i < m_ids->size(); ++i)
    {
        std::cout << m_ids->operator[](i);

        if (i + 1 != m_ids->size())
        {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

DeclarationBlock::DeclarationBlock() : Node(), m_declarations()
{
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

BinaryExpression::BinaryExpression(Expression *exp1, Expression *exp2,
                                   Operator op)
    : m_exp1(exp1), m_exp2(exp2), m_op(op)
{
}

void BinaryExpression::print_repr() const
{
    std::cout << "BinaryExpression<" << to_string(m_op) << ">\n";
    std::cout << "    ";
    m_exp1->print_repr();
    std::cout << "\n";
    std::cout << "    ";
    m_exp2->print_repr();
    std::cout << "\n";
}
NumberExpression::NumberExpression(Number number) : m_number(number)
{
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

CastExpression::CastExpression(NumberType cast_to, Expression *casted_expr)
    : m_cast(cast_to), m_expression(casted_expr)
{
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

IdExpression::IdExpression(char *id_name) : m_id(id_name)
{
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

bool BinaryBoolExpression::evaluate() const
{
    // TODO
    return false;
}
