#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Types.hpp"

class Node;
class Statement;
class StatementList;
class Declaration;
class DeclarationBlock;

//===----------------------------------------------------------------------===//

class Node
{
public:
    Node()                          = default;
    virtual ~Node()                 = default;
    virtual void print_repr() const = 0;
};

//===----------------------------------------------------------------------===//

class Declaration : public Node
{
public:
    Declaration() = default;

    void print_repr() const override;
    void add_id(std::string *id)
    {
        m_ids.push_back(id);
    }
    void assign_number_type(NumberType type)
    {
        m_type = type;
    }

private:
    std::vector<std::string *> m_ids;
    NumberType                 m_type;
};

class DeclarationBlock : public Node
{
public:
    DeclarationBlock() = default;
    void print_repr() const override;
    void add_declaration(Declaration *decl);

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
    Operator    m_op;
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
    NumberType  m_cast;
    Expression *m_expression;
};

//===----------------------------------------------------------------------===//

class IdExpression : public Expression
{
public:
    IdExpression(std::string *id_name)
        : Expression()
        , m_id(id_name)
    {
    }
    void print_repr() const override;

private:
    std::string *m_id;
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
    Operator    m_op;
};

//===----------------------------------------------------------------------===//

class Statement : public Node
{
public:
    void print_repr() const override
    {
        std::cout << "Statement<?>\n";
    }
};

//===----------------------------------------------------------------------===//

class AssignStatement : public Statement
{
public:
    AssignStatement(std::string *id, Expression *exp)
        : m_id(id)
        , m_exp(exp)
    {
    }

    void print_repr() const override;

private:
    std::string *m_id;
    Expression  *m_exp;
};

class OutputStatement : public Statement
{
public:
    OutputStatement(Expression *exp)
        : Statement()
        , m_exp(exp)
    {
    }

private:
    Expression *m_exp;
};

class InputStatement : public Statement
{
public:
    InputStatement(std::string *id)
        : m_id(id)
    {
    }

    void print_repr() const override;

private:
    std::string *m_id;
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

    void print_repr() const override;

private:
    BoolExpression *m_condition_exp;
    Statement      *m_then_branch;
    Statement      *m_else_branch;
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

    void print_repr() const override
    {
        std::cout << "while (TODO)\n";
    }

private:
    BoolExpression *m_condition_exp;
    Statement      *m_body_statement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;
    void print_repr() const override
    {
        std::cout << "Break\n";
    }
};

class SwitchStatement : public Statement
{
public:
    SwitchStatement()
        : Statement()
    {
    }

    void add_case(Number case_num, StatementList *case_statements)
    {
        m_cases.emplace_back(case_num, case_statements);
    }

    void set_expression(Expression *exp)
    {
        m_exp = exp;
    }

    void set_default_case(StatementList *default_case_stmt_list)
    {
        m_default_case = default_case_stmt_list;
    }

    void print_repr() const override
    {
        std::cout << "Switch<TODO>\n";
    }

private:
    Expression                                     *m_exp;
    std::vector<std::pair<Number, StatementList *>> m_cases;
    StatementList                                  *m_default_case = nullptr;
};

//===----------------------------------------------------------------------===//

class StatementList : public Statement
{
public:
    StatementList() = default;
    void add_statement(Statement *stmt_ptr)
    {
        m_statements.push_back(stmt_ptr);
    }

    void print_repr() const override
    {
        std::cout << "StatementList<" << m_statements.size() << ">:\n";
        for (auto const stmt : m_statements)
        {
            std::cout << "    ";
            stmt->print_repr();
        }
    }

private:
    std::vector<Statement *> m_statements;
};

class Program : public Node
{
public:
    Program() = default;

    void set_declaration_block(DeclarationBlock *db)
    {
        m_declaration_block = db;
    }

    void set_program_statements(Statement *statement)
    {
        m_program_statements = statement;
    }

    void print_repr() const override
    {
        std::cout << "//===- Declarations -------===//\n";
        m_declaration_block->print_repr();
        std::cout << "//===- Statements -------===//\n";
        m_program_statements->print_repr();
    }

private:
    DeclarationBlock *m_declaration_block;
    Statement        *m_program_statements = nullptr;
};
