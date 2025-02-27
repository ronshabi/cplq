#pragma once

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



class NumberExpression : public Expression 
{
public:
    NumberExpression(Number number);
    void print_repr() const override;

private:
    Number m_number;
};

class CastExpression : public Expression
{
public:
    CastExpression(NumberType cast_to, Expression *casted_expr);
    void print_repr() const override;

private:
    NumberType m_cast;
    Expression *m_expression;
};

class IdExpression : public Expression {
public:
    IdExpression(std::string *id_name);
    void print_repr() const override;

private:
    std::string *m_id;
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(Expression* exp1, Expression *exp2, Operator op);
    void print_repr() const override;

private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator m_op;
};
} // namespace cpl::ast
