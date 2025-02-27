#pragma once

#include <memory>
#include <string>
#include <vector>

#include "types.hxx"

namespace cpl::ast
{

class Node
{
public:
    Node() = default;
    virtual ~Node() = default;
    virtual void print_repr() const = 0;
};

class Declaration : public Node
{
public:
    Declaration(std::vector<std::string> *id_vec_ptr,
                cpl::NumberType decl_type);
    void print_repr() const override;

private:
    std::vector<std::string> *m_ids;
    cpl::NumberType m_type;
};

class DeclarationBlock : public Node
{
public:
    DeclarationBlock();
    void print_repr() const override;
    void add_declaration(Declaration *declaration);    
private:
    std::vector<Declaration *> m_declarations;
};

class Expression : public Node
{
};

class UnaryExpression : public Expression 
{

};

class BinaryExpression : public Expression
{

};
} // namespace cpl::ast
