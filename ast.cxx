#include "ast.hxx"

#include <iostream>

namespace cpl::ast
{

Decl::Decl(std::vector<std::string> *id_vec_ptr, cpl::NumberType decl_type)
    : ASTNode()
    , m_ids(id_vec_ptr)
    , m_type(decl_type)
{
}

void Decl::print_repr() const {
  std::cout << "Decl<" << to_string(m_type) << "> [";
  for (std::size_t i = 0; i < m_ids->size(); ++i) {
    std::cout << m_ids->operator[](i);

    if (i + 1 != m_ids->size()) {
      std::cout << ", ";
    }
  }

  std::cout << "]\n";
}
}    // namespace cpl::ast
