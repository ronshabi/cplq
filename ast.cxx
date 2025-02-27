#include "ast.hxx"

#include <iostream>

namespace cpl::ast
{

Declaration::Declaration(std::vector<std::string> *id_vec_ptr,
                         cpl::NumberType decl_type)
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

DeclarationBlock::DeclarationBlock()
    : Node(),
      m_declarations()
{
    
}

void DeclarationBlock::print_repr() const {
    std::cout << "DeclarationBlock<" << m_declarations.size() << ">:\n";
    for (auto const& decl : m_declarations) {
        std::cout << "  ";
        decl->print_repr();
    }
}

void DeclarationBlock::add_declaration(Declaration *declaration) {
    m_declarations.emplace_back(declaration);    
}
} // namespace cpl::ast
