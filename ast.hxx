#pragma once

#include <memory>
#include <string>
#include <vector>

#include "types.hxx"

namespace cpl::ast
{

class ASTNode
{
  public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual void print_repr() const = 0;
};

class Decl : public ASTNode
{
  public:
    Decl(std::vector<std::string> *id_vec_ptr, cpl::NumberType decl_type);
    void print_repr() const override;
  private:
    std::unique_ptr<std::vector<std::string>> m_ids;
    cpl::NumberType                           m_type;
};

class Expression : public ASTNode
{
};
}    // namespace cpl::ast
