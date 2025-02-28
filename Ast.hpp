#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

//===----------------------------------------------------------------------===//

class Node
{
public:
    Node() = default;
    virtual ~Node() = default;
    virtual void print_repr() const = 0;
};

//===----------------------------------------------------------------------===//

class Declaration : public Node
{
public:
    Declaration(std::vector<std::string> *id_vec_ptr, NumberType decl_type);
    void print_repr() const override;

private:
    std::vector<std::string> *m_ids;
    NumberType m_type;
};

//===----------------------------------------------------------------------===//

class DeclarationBlock : public Node
{
public:
    DeclarationBlock();
    void print_repr() const override;
    void add_declaration(Declaration *declaration);

private:
    std::vector<Declaration *> m_declarations;
};

//===----------------------------------------------------------------------===//

class Expression : public Node
{
};

//===----------------------------------------------------------------------===//

class NumberExpression : public Expression
{
public:
    NumberExpression(Number number);
    void print_repr() const override;

private:
    Number m_number;
};

//===----------------------------------------------------------------------===//

class CastExpression : public Expression
{
public:
    CastExpression(NumberType cast_to, Expression *casted_expr);
    void print_repr() const override;

private:
    NumberType m_cast;
    Expression *m_expression;
};

//===----------------------------------------------------------------------===//

class IdExpression : public Expression
{
public:
    IdExpression(char *id_name);
    void print_repr() const override;

private:
    char *m_id;
};

//===----------------------------------------------------------------------===//

class BinaryExpression : public Expression
{
public:
    BinaryExpression(Expression *exp1, Expression *exp2, Operator op);
    void print_repr() const override;

private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator m_op;
};

//===----------------------------------------------------------------------===//

class NotExpression : public Expression
{
public:
    NotExpression(Expression *exp) : m_exp(exp)
    {
    }
    void print_repr() const override;

private:
    Expression *m_exp;
};

//===----------------------------------------------------------------------===//

class Statement : public Node
{
};

//===----------------------------------------------------------------------===//

class AssignStatement : public Statement
{
public:
    AssignStatement(char *id, Expression *exp) : m_id(id), m_exp(exp)
    {
    }

private:
    char *m_id;
    Expression *m_exp;
};

//===----------------------------------------------------------------------===//

class StatementList : public Node
{
public:
    StatementList() = default;

private:
    std::vector<Statement *> m_statements;
};