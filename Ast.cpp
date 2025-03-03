#include "Ast.hpp"

void DeclarationBlock::add_declaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

bool BinaryBoolExpression::evaluate() const
{
    // TODO
    return false;
}
