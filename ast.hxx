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

class Declaration : public ASTNode
{
public:
    Declaration(std::vector<std::string> *id_vec_ptr,
                cpl::NumberType decl_type);
    void print_repr() const override;

private:
    std::unique_ptr<std::vector<std::string>> m_ids;
    cpl::NumberType m_type;
};

class DeclarationBlock : public ASTNode
{
public:
    DeclarationBlock();
    void print_repr() const override;
    void add_declaration(Declaration *declaration);    
private:
    std::vector<std::unique_ptr<Declaration>> m_declarations;
};

class Expression : public ASTNode

{
};
} // namespace cpl::ast
