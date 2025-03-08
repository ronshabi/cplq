#pragma once

#include <string>
#include <vector>

#include <iostream>

#include "Types.hpp"
#include "Utils.hpp"

class AstNode;
class Statement;
class StatementList;
class Declaration;
class DeclarationBlock;
class SemanticAnalyzer;
//===----------------------------------------------------------------------===//

class AstNode
{
public:
    AstNode()          = default;
    virtual ~AstNode() = default;

    virtual void dump(unsigned indent = 0) const;
    virtual void accept_semantic_analyzer(SemanticAnalyzer &sema) = 0;
};

//===----------------------------------------------------------------------===//

class Declaration : public AstNode
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

    std::vector<std::string *> const& get_ids() const { return m_ids; }
    NumberType get_type() const { return m_type; }

private:
    std::vector<std::string *> m_ids;
    NumberType                 m_type;
};

class DeclarationBlock : public AstNode
{
public:
    DeclarationBlock() = default;
    void add_declaration(Declaration *decl);

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

    std::vector<Declaration *>& get_declarations() { return m_declarations; }

private:
    std::vector<Declaration *> m_declarations;
};

//===----------------------------------------------------------------------===//

class Expression : public AstNode
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator    m_op;
};

//===----------------------------------------------------------------------===//

class Statement : public AstNode
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;


private:
    BoolExpression *m_condition_exp;
    Statement      *m_body_statement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

    std::vector<Statement *> &get_statements()
    {
        return m_statements;
    }

private:
    std::vector<Statement *> m_statements;
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

private:
    Expression                                 *m_exp;
    std::vector<std::pair<Number, Statement *>> m_cases;
    Statement                                  *m_default_case = nullptr;
};

class Program : public AstNode
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;

    DeclarationBlock *get_declaration_block()
    {
        return m_declaration_block;
    }
    Statement *get_statemenets()
    {
        return m_program_statements;
    }

private:
    DeclarationBlock *m_declaration_block;
    Statement        *m_program_statements = nullptr;
};
