#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Register.hpp"
#include "SymbolTable.hpp"
#include "Types.hpp"
#include "Utils.hpp"

// Forward decls.
class AstNode;
class Statement;
class StatementList;
class Declaration;
class DeclarationBlock;
class SemanticAnalyzer;
class CodeGenerator;

//===----------------------------------------------------------------------===//

/// \class AstNode
/// \brief Base class for all AST nodes.
class AstNode
{
public:
    AstNode()          = default;
    virtual ~AstNode() = default;

    virtual void dump(unsigned indent = 0) const;
    virtual void accept_semantic_analyzer(SemanticAnalyzer &sema) = 0;
    virtual void accept_code_generator(CodeGenerator &cg)         = 0;

    unsigned
    get_line_number() const
    {
        return m_line_no;
    }

    void
    set_line_number(unsigned line_no)
    {
        m_line_no = line_no;
    }

    Register
    get_register() const
    {
        return m_register;
    }

    friend CodeGenerator;

protected:
    Register m_register{};

private:
    unsigned m_line_no = 0;
};

//===----------------------------------------------------------------------===//

/// \class Declaration
/// \brief Represents a variable declaration.
class Declaration : public AstNode
{
public:
    Declaration() = default;

    void
    add_id(std::string *id)
    {
        m_ids.push_back(id);
    }

    void
    assign_number_type(NumberType type)
    {
        m_type = type;
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    std::vector<std::string *> const &
    get_ids() const
    {
        return m_ids;
    }

    NumberType
    get_type() const
    {
        return m_type;
    }

private:
    std::vector<std::string *> m_ids;
    NumberType                 m_type;
};

//===----------------------------------------------------------------------===//

/// \class DeclarationBlock
/// \brief Represents a block of variable declarations.
class DeclarationBlock : public AstNode
{
public:
    DeclarationBlock() = default;
    void add_declaration(Declaration *decl);

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    std::vector<Declaration *> &
    get_declarations()
    {
        return m_declarations;
    }

private:
    std::vector<Declaration *> m_declarations;
};

//===----------------------------------------------------------------------===//

/// \class Expression
/// \brief Base class for expressions.
class Expression : public AstNode
{
public:
    virtual bool
    is_bool_expr() const
    {
        return false;
    }
    virtual NumberType
    get_type(SymbolTable const &symbol_table) const
    {
        UNUSED(symbol_table);
        return NumberType::INT;
    } // default type
};

//===----------------------------------------------------------------------===//

/// \class BoolExpression
/// \brief Base class for boolean expressions.
class BoolExpression : public Expression
{
public:
    virtual bool
    is_bool_expr() const
    {
        return true;
    }
};

/// \class NotExpression
/// \brief Represents a logical NOT expression.
class NotExpression : public BoolExpression
{
public:
    NotExpression(BoolExpression *exp)
        : BoolExpression()
        , m_exp(exp)
    {
    }

    Expression *
    get_expression()
    {
        return m_exp;
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

private:
    BoolExpression *m_exp;
};

//===----------------------------------------------------------------------===//

/// \class BinaryBoolExpression
/// \brief Represents a binary boolean expression.
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

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    Expression *
    get_lhs_expr()
    {
        return m_exp1;
    }
    Expression *
    get_rhs_expr()
    {
        return m_exp2;
    }
    Operator
    get_operator() const
    {
        return m_op;
    }

private:
    Operator    m_op;
    Expression *m_exp1;
    Expression *m_exp2;
};

//===----------------------------------------------------------------------===//

/// \class NumberExpression
/// \brief Represents a number literal expression.
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
    void accept_code_generator(CodeGenerator &cg) override;

    NumberType
    get_type(SymbolTable const &symbol_table) const override
    {
        UNUSED(symbol_table);
        return m_number.type;
    }

    Number
    get_number() const
    {
        return m_number;
    }

private:
    Number m_number;
};

//===----------------------------------------------------------------------===//

/// \class CastExpression
/// \brief Represents a type cast expression.
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
    void accept_code_generator(CodeGenerator &cg) override;

    NumberType
    get_type(SymbolTable const &symbol_table) const override
    {
        UNUSED(symbol_table);
        return m_cast;
    }

    Expression *
    get_expression()
    {
        return m_expression;
    }

private:
    NumberType  m_cast;
    Expression *m_expression;
};

//===----------------------------------------------------------------------===//

/// \class IdExpression
/// \brief Represents an identifier expression.
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
    void accept_code_generator(CodeGenerator &cg) override;

    std::string *
    get_id() const
    {
        return m_id;
    }

    NumberType get_type(SymbolTable const &symbol_table) const override;

private:
    std::string *m_id;
};

//===----------------------------------------------------------------------===//

/// \class BinaryExpression
/// \brief Represents a binary arithmetic expression.
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
    void accept_code_generator(CodeGenerator &cg) override;

    NumberType get_type(SymbolTable const &symbol_table) const override;

    Expression *
    get_lhs_expr()
    {
        return m_exp1;
    }
    Expression *
    get_rhs_expr()
    {
        return m_exp2;
    }
    Operator
    get_operator() const
    {
        return m_op;
    }

private:
    Expression *m_exp1;
    Expression *m_exp2;
    Operator    m_op;
};

//===----------------------------------------------------------------------===//

/// \class Statement
/// \brief Base class for statements.
class Statement : public AstNode
{
public:
    virtual bool
    is_break_statement() const
    {
        return false;
    }
};

//===----------------------------------------------------------------------===//

/// \class AssignStatement
/// \brief Represents an assignment statement.
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
    void accept_code_generator(CodeGenerator &cg) override;

    std::string *
    get_id() const
    {
        return m_id;
    }
    Expression *
    get_expression()
    {
        return m_exp;
    }

private:
    std::string *m_id;
    Expression  *m_exp;
};

//===----------------------------------------------------------------------===//

/// \class OutputStatement
/// \brief Represents an output statement.
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
    void accept_code_generator(CodeGenerator &cg) override;

    Expression *
    get_expression()
    {
        return m_exp;
    }

private:
    Expression *m_exp;
};

//===----------------------------------------------------------------------===//

/// \class InputStatement
/// \brief Represents an input statement.
class InputStatement : public Statement
{
public:
    InputStatement(std::string *id)
        : m_id(id)
    {
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    std::string *
    get_id() const
    {
        return m_id;
    }

private:
    std::string *m_id;
};

//===----------------------------------------------------------------------===//

/// \class IfStatement
/// \brief Represents an if statement.
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
    void accept_code_generator(CodeGenerator &cg) override;

    BoolExpression *
    get_condition_exp()
    {
        return m_condition_exp;
    }
    Statement *
    get_then_branch()
    {
        return m_then_branch;
    }
    Statement *
    get_else_branch()
    {
        return m_else_branch;
    }

private:
    BoolExpression *m_condition_exp;
    Statement      *m_then_branch;
    Statement      *m_else_branch;
};

//===----------------------------------------------------------------------===//

/// \class WhileStatement
/// \brief Represents a while statement.
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
    void accept_code_generator(CodeGenerator &cg) override;

    BoolExpression *
    get_condition_expression()
    {
        return m_condition_exp;
    }
    Statement *
    get_body_statement()
    {
        return m_body_statement;
    }

private:
    BoolExpression *m_condition_exp;
    Statement      *m_body_statement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;

    bool
    is_break_statement() const override
    {
        return true;
    }
    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;
};

//===----------------------------------------------------------------------===//

/// \class StatementList
/// \brief Represents a list of statements.
class StatementList : public Statement
{
public:
    StatementList() = default;
    void
    add_statement(Statement *stmt_ptr)
    {
        m_statements.push_back(stmt_ptr);
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    std::vector<Statement *> &
    get_statements()
    {
        return m_statements;
    }

private:
    std::vector<Statement *> m_statements;
};

//===----------------------------------------------------------------------===//

/// \class SwitchStatement
/// \brief Represents a switch statement.
class SwitchStatement : public Statement
{
public:
    SwitchStatement()
        : Statement()
    {
    }

    /// \brief Add a case to the switch statement.
    /// \param case_num The case number.
    /// \param case_statements The statements for the case.
    void
    add_case(Number case_num, StatementList *case_statements)
    {
        m_cases.emplace_back(case_num, case_statements);
    }

    /// \brief Set the expression for the switch statement.
    /// \param exp The expression.
    void
    set_expression(Expression *exp)
    {
        m_exp = exp;
    }

    /// \brief Set the default case for the switch statement.
    /// \param default_case_stmt_list The statements for the default case.
    void
    set_default_case(StatementList *default_case_stmt_list)
    {
        m_default_case = default_case_stmt_list;
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    Expression *
    get_expression()
    {
        return m_exp;
    }
    std::vector<std::pair<Number, Statement *>> &
    get_cases()
    {
        return m_cases;
    }
    Statement *
    get_default_case()
    {
        return m_default_case;
    }

private:
    /// \brief The expression being switched on.
    /// This expression's value is compared against the case values.
    /// It must evaluate to a number type.
    Expression *m_exp;

    /// \brief The vector of cases in the switch statement.
    /// Each case consists of a number value and a list of statements.
    /// The cases are stored in the order they were added.
    std::vector<std::pair<Number, Statement *>> m_cases;

    /// \brief The default case for the switch statement.
    /// This case is executed if no other case matches the expression value.
    Statement *m_default_case = nullptr;
};

//===----------------------------------------------------------------------===//

/// \class Program
/// \brief Represents a complete program.
class Program : public AstNode
{
public:
    Program() = default;

    /// \brief Set the declaration block for the program.
    /// \param db The declaration block.
    void
    set_declaration_block(DeclarationBlock *db)
    {
        m_declaration_block = db;
    }

    /// \brief Set the program statements.
    /// \param statement The program statements.
    void
    set_program_statements(Statement *statement)
    {
        m_program_statements = statement;
    }

    void dump(unsigned indent) const override;
    void accept_semantic_analyzer(SemanticAnalyzer &sema) override;
    void accept_code_generator(CodeGenerator &cg) override;

    DeclarationBlock *
    get_declaration_block()
    {
        return m_declaration_block;
    }
    Statement *
    get_statemenets()
    {
        return m_program_statements;
    }

private:
    DeclarationBlock *m_declaration_block;
    Statement        *m_program_statements = nullptr;
};
