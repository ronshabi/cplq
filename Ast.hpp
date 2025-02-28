#pragma once

#include <iostream>
#include <memory>
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
    Declaration(std::vector<std::string> *id_vec_ptr, NumberType decl_type)
        : Node()
        , m_ids(id_vec_ptr)
        , m_type(decl_type)
    {
    }

    void print_repr() const override;

private:
    std::vector<std::string> *m_ids;
    NumberType m_type;
};

//===----------------------------------------------------------------------===//

class DeclarationBlock : public Node
{
public:
    DeclarationBlock()
        : Node()
        , m_declarations()
    {
    }

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

class BoolExpression : public Expression
{
public:
    virtual bool evaluate() const = 0;
};

class NotExpression : public BoolExpression
{
public:
    NotExpression(BoolExpression *exp)
        : BoolExpression()
        , m_exp(exp)
    {
    }

    void print_repr() const override;

    bool evaluate() const override
    {
        return !m_exp->evaluate();
    }

private:
    BoolExpression *m_exp;
};

class BinaryBoolExpression : public BoolExpression
{
public:
    BinaryBoolExpression(Expression *e1, Expression *e2, Operator op)
        : BoolExpression()
        , m_op(op)
        , m_exp1(e1)
        , m_exp2(e2)
    {
    }
    void print_repr() const override;
    bool evaluate() const override;

private:
    Operator m_op;
    Expression *m_exp1;
    Expression *m_exp2;
};

//===----------------------------------------------------------------------===//

class NumberExpression : public Expression
{
public:
    NumberExpression(Number number)
        : Expression()
        , m_number(number)
    {
    }

    void print_repr() const override;

private:
    Number m_number;
};

//===----------------------------------------------------------------------===//

class CastExpression : public Expression
{
public:
    CastExpression(NumberType cast_to, Expression *casted_expr)
        : Expression()
        , m_cast(cast_to)
        , m_expression(casted_expr)
    {
    }

    void print_repr() const override;

private:
    NumberType m_cast;
    Expression *m_expression;
};

//===----------------------------------------------------------------------===//

class IdExpression : public Expression
{
public:
    IdExpression(char *id_name)
        : Expression()
        , m_id(id_name)
    {
    }
    void print_repr() const override;

private:
    char *m_id;
};

//===----------------------------------------------------------------------===//

class BinaryExpression : public Expression
{
public:
    BinaryExpression(Expression *exp1, Expression *exp2, Operator op)
        : Expression()
        , m_exp1(exp1)
        , m_exp2(exp2)
        , m_op(op)
    {
    }
    void print_repr() const override;

private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator m_op;
};

//===----------------------------------------------------------------------===//

class Statement : public Node
{
};

//===----------------------------------------------------------------------===//

class AssignStatement : public Statement
{
public:
    AssignStatement(char *id, Expression *exp)
        : m_id(id)
        , m_exp(exp)
    {
    }

private:
    char *m_id;
    Expression *m_exp;
};

class IOStatement : public Statement
{
public:
    enum class Type
    {
        Input,
        Output
    };

    IOStatement(Type type, Expression *exp)
        : Statement()
        , m_type(type)
        , m_exp(exp)
    {
    }

private:
    Type m_type;
    Expression *m_exp;
};

class IfStatement : public Statement
{
public:
    IfStatement(BoolExpression *condition, Statement *then_stmt,
                Statement *else_stmt)
        : Statement()
        , m_condition_exp(condition)
        , m_then_branch(then_stmt)
        , m_else_branch(else_stmt)
    {
    }

private:
    BoolExpression *m_condition_exp;
    Statement *m_then_branch;
    Statement *m_else_branch;
};

class WhileStatement : public Statement
{
public:
    WhileStatement(BoolExpression *condition, Statement *body)
        : Statement()
        , m_condition_exp(condition)
        , m_body_statement(body)
    {
    }

private:
    BoolExpression *m_condition_exp;
    Statement *m_body_statement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;
};

//===----------------------------------------------------------------------===//

class StatementList : public Node
{
public:
    StatementList() = default;

private:
    std::vector<Statement *> m_statements;
};