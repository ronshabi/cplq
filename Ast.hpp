#pragma once

#include <string>
#include <vector>

#include <iostream>

#include "Types.hpp"
#include "Utils.hpp"

class Node;
class Statement;
class StatementList;
class Declaration;
class DeclarationBlock;

//===----------------------------------------------------------------------===//

class Node
{
public:
    Node()                     = default;
    virtual ~Node()            = default;

    virtual void dump(unsigned indent = 0) const {
        for (unsigned i = 0; i < indent; ++i) {
            std::cout << ' ';
        }
    }
};

//===----------------------------------------------------------------------===//

class Declaration : public Node
{
public:
    Declaration() = default;

    void add_id(std::string *id)
    {
        m_ids.push_back(id);
    }
    void assign_number_type(NumberType type)
    {
        m_type = type;
    }

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Decl<" << number_type_to_string(m_type) << "> " << vector_to_string(m_ids) << "\n";
    }


private:
    std::vector<std::string *> m_ids;
    NumberType                 m_type;
};

class DeclarationBlock : public Node
{
public:
    DeclarationBlock() = default;
    void add_declaration(Declaration *decl);

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "DeclBlock<" << m_declarations.size() << ">:\n";
        for (const auto& decl : m_declarations) {
            decl->dump(indent + 4);
        }
    }

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

    bool evaluate() const override
    {
        return !m_exp->evaluate();
    }

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "NotExp:\n";
        m_exp->dump(indent + 4);
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
    bool evaluate() const override;

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "BinaryBoolExpression<" << operator_to_string(m_op) << ">:\n";
        m_exp1->dump(indent + 4);
        m_exp2->dump(indent + 4);
    }

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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        if (m_number.type == NumberType::FLOAT) {
            std::cout << "Number<Float>(" << m_number.d << ")\n";
        } else {
            std::cout << "Number<Int>(" << m_number.i << ")\n";
        }
    }

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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Cast<" << number_type_to_string(m_cast) << ">:\n";
        m_expression->dump(indent+4);
    }
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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Id<" << *m_id << ">\n";
    }
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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "BinaryExpression<" << operator_to_string(m_op) << ">:\n";
        m_exp1->dump(indent + 4);
        m_exp2->dump(indent + 4);
    }

private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator    m_op;
};

//===----------------------------------------------------------------------===//

class Statement : public Node
{
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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Assign<" << *m_id << ">:\n";
        m_exp->dump(indent+4);
    }

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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Output:\n";
        m_exp->dump(indent+4);
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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "Input<" << *m_id << ">\n";
    }

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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "If:\n";
        m_condition_exp->dump(indent+4);
        m_then_branch->dump(indent+4);
        m_else_branch->dump(indent+4);
    }


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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "While:\n";
        m_condition_exp->dump(indent+4);
        m_body_statement->dump(indent+4);
    }

private:
    BoolExpression *m_condition_exp;
    Statement      *m_body_statement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;

    void dump(unsigned indent) const override {
        Node::dump(indent);
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

    
    void dump(unsigned indent) const override {
        Node::dump(indent);
        std::cout << "StatementList:\n";
        for (auto const& stmt : m_statements) {
            stmt->dump(indent+4);
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

    void dump(unsigned indent) const override {
        Node::dump(indent);
        m_declaration_block->dump(0);
        m_program_statements->dump(0);
    }


private:
    DeclarationBlock *m_declaration_block;
    Statement        *m_program_statements = nullptr;
};
