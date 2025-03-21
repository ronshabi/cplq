//===-- Ast.hpp -----------------------------------------------------------===//
//
// Definition of the AST & all ASTNodes of the CPL language.
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi.
//
//===----------------------------------------------------------------------===//
#pragma once

#include <string>
#include <vector>

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

    virtual void Dump(unsigned indent = 0) const;
    virtual void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) = 0;
    virtual void AcceptCodeGenerator(CodeGenerator &cg)         = 0;

    unsigned
    GetLineNumber() const
    {
        return m_lineNo;
    }

    void
    SetLineNumber(unsigned line_no)
    {
        m_lineNo = line_no;
    }

    Register
    GetRegister() const
    {
        return m_register;
    }

    friend CodeGenerator;

protected:
    Register m_register{};

private:
    unsigned m_lineNo = 0;
};

//===----------------------------------------------------------------------===//

/// \class Declaration
/// \brief Represents a variable declaration.
class Declaration : public AstNode
{
public:
    Declaration() = default;
    ~Declaration();

    void
    AddID(std::string *id)
    {
        m_ids.push_back(id);
    }

    void
    AssignNumberType(NumberType type)
    {
        m_type = type;
    }

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::vector<std::string *> const &
    GetIDs() const
    {
        return m_ids;
    }

    NumberType
    GetType() const
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
    ~DeclarationBlock();

    void AddDeclaration(Declaration *decl);

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::vector<Declaration *> &
    GetDeclarations()
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
    IsBoolExpr() const
    {
        return false;
    }
    virtual NumberType
    GetType(SymbolTable const &symbol_table) const
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
    IsBoolExpr() const
    {
        return true;
    }
};

/// \class NotExpression
/// \brief Represents a logical NOT expression.
class NotExpression : public BoolExpression
{
public:
    NotExpression(BoolExpression *expr);
    ~NotExpression();

    Expression *
    GetExpr()
    {
        return m_expr;
    }

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

private:
    BoolExpression *m_expr;
};

//===----------------------------------------------------------------------===//

/// \class BinaryBoolExpression
/// \brief Represents a binary boolean expression.
class BinaryBoolExpression : public BoolExpression
{
public:
    BinaryBoolExpression(Expression *e1, Expression *e2, Operator op);
    ~BinaryBoolExpression();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    Expression *
    GetLhsExpr()
    {
        return m_lhsExpr;
    }
    Expression *
    GetRhsExpr()
    {
        return m_rhsExpr;
    }
    Operator
    GetOperator() const
    {
        return m_op;
    }

private:
    Operator    m_op;
    Expression *m_lhsExpr;
    Expression *m_rhsExpr;
};

//===----------------------------------------------------------------------===//

/// \class NumberExpression
/// \brief Represents a number literal expression.
class NumberExpression : public Expression
{
public:
    NumberExpression(Number number);

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    NumberType
    GetType(SymbolTable const &symbol_table) const override
    {
        UNUSED(symbol_table);
        return m_number.type;
    }

    Number
    GetNumber() const
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
    CastExpression(NumberType cast_to, Expression *casted_expr);
    ~CastExpression();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    NumberType
    GetType(SymbolTable const &symbol_table) const override
    {
        UNUSED(symbol_table);
        return m_cast;
    }

    Expression *
    GetExpr()
    {
        return m_expr;
    }

private:
    NumberType  m_cast;
    Expression *m_expr;
};

//===----------------------------------------------------------------------===//

/// \class IdExpression
/// \brief Represents an identifier expression.
class IdExpression : public Expression
{
public:
    IdExpression(std::string *id_name);
    ~IdExpression();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::string *
    GetID() const
    {
        return m_id;
    }

    NumberType GetType(SymbolTable const &symbol_table) const override;

private:
    std::string *m_id;
};

//===----------------------------------------------------------------------===//

/// \class BinaryExpression
/// \brief Represents a binary arithmetic expression.
class BinaryExpression : public Expression
{
public:
    BinaryExpression(Expression *exp1, Expression *exp2, Operator op);
    ~BinaryExpression();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    NumberType GetType(SymbolTable const &symbolTable) const override;

    Expression *
    GetLhsExpr()
    {
        return m_lhsExpr;
    }
    Expression *
    GetRhsExpr()
    {
        return m_rhsExpr;
    }
    Operator
    GetOperator() const
    {
        return m_op;
    }

private:
    Expression *m_lhsExpr;
    Expression *m_rhsExpr;
    Operator    m_op;
};

//===----------------------------------------------------------------------===//

/// \class Statement
/// \brief Base class for statements.
class Statement : public AstNode
{
public:
    virtual bool
    IsBreakStatement() const
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
    AssignStatement(std::string *id, Expression *expr);
    ~AssignStatement();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::string *
    GetID() const
    {
        return m_id;
    }
    Expression *
    GetExpr()
    {
        return m_expr;
    }

private:
    std::string *m_id;
    Expression  *m_expr;
};

//===----------------------------------------------------------------------===//

/// \class OutputStatement
/// \brief Represents an output statement.
class OutputStatement : public Statement
{
public:
    OutputStatement(Expression *expr);
    ~OutputStatement();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    Expression *
    GetExpr()
    {
        return m_expr;
    }

private:
    Expression *m_expr;
};

//===----------------------------------------------------------------------===//

/// \class InputStatement
/// \brief Represents an input statement.
class InputStatement : public Statement
{
public:
    InputStatement(std::string *id);
    ~InputStatement();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::string *
    GetID() const
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
                Statement *else_stmt);

    ~IfStatement();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    BoolExpression *
    GetConditionExpr()
    {
        return m_conditionExpr;
    }
    Statement *
    GetThenBranch()
    {
        return m_thenBranch;
    }
    Statement *
    GetElseBranch()
    {
        return m_elseBranch;
    }

private:
    BoolExpression *m_conditionExpr;
    Statement      *m_thenBranch;
    Statement      *m_elseBranch;
};

//===----------------------------------------------------------------------===//

/// \class WhileStatement
/// \brief Represents a while statement.
class WhileStatement : public Statement
{
public:
    WhileStatement(BoolExpression *condition, Statement *body);
    ~WhileStatement();

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    BoolExpression *
    GetConditionExpr()
    {
        return m_conditionExpr;
    }
    Statement *
    GetBodyStatement()
    {
        return m_bodyStatement;
    }

private:
    BoolExpression *m_conditionExpr;
    Statement      *m_bodyStatement;
};

class BreakStatement : public Statement
{
public:
    BreakStatement() = default;

    bool
    IsBreakStatement() const override
    {
        return true;
    }
    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;
};

//===----------------------------------------------------------------------===//

/// \class StatementList
/// \brief Represents a list of statements.
class StatementList : public Statement
{
public:
    StatementList() = default;
    ~StatementList();
    
    void
    AddStatement(Statement *stmt)
    {
        m_statements.push_back(stmt);
    }

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    std::vector<Statement *> &
    GetStatements()
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
    SwitchStatement() = default;
    ~SwitchStatement();

    

    /// \brief Add a case to the switch statement.
    /// \param caseNum The case number.
    /// \param caseStatements The statements for the case.
    void
    AddCase(Number caseNum, StatementList *caseStatements)
    {
        m_cases.emplace_back(caseNum, caseStatements);
    }

    /// \brief Set the expression for the switch statement.
    /// \param expr The expression.
    void
    SetExpr(Expression *expr)
    {
        m_expr = expr;
    }

    /// \brief Set the default case for the switch statement.
    /// \param defaultCaseStatementList The statements for the default case.
    void
    SetDefaultCase(StatementList *defaultCaseStatementList)
    {
        m_defaultCase = defaultCaseStatementList;
    }

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    Expression *
    GetExpr()
    {
        return m_expr;
    }
    std::vector<std::pair<Number, Statement *>> &
    GetCases()
    {
        return m_cases;
    }
    Statement *
    GetDefaultCase()
    {
        return m_defaultCase;
    }

private:
    /// \brief The expression being switched on.
    /// This expression's value is compared against the case values.
    /// It must evaluate to a number type.
    Expression *m_expr;

    /// \brief The vector of cases in the switch statement.
    /// Each case consists of a number value and a list of statements.
    /// The cases are stored in the order they were added.
    std::vector<std::pair<Number, Statement *>> m_cases;

    /// \brief The default case for the switch statement.
    /// This case is executed if no other case matches the expression value.
    Statement *m_defaultCase = nullptr;
};

//===----------------------------------------------------------------------===//

/// \class Program
/// \brief Represents a complete program.
class Program : public AstNode
{
public:
    Program() = default;
    ~Program();

    /// \brief Set the declaration block for the program.
    /// \param db The declaration block.
    void
    SetDeclarationBlock(DeclarationBlock *db)
    {
        m_declarationBlock = db;
    }

    /// \brief Set the program statements.
    /// \param statement The program statements.
    void
    SetProgramStatements(Statement *statement)
    {
        m_programStatements = statement;
    }

    void Dump(unsigned indent) const override;
    void AcceptSemanticAnalyzer(SemanticAnalyzer &sema) override;
    void AcceptCodeGenerator(CodeGenerator &cg) override;

    DeclarationBlock *
    GetDeclarationBlock()
    {
        return m_declarationBlock;
    }
    Statement *
    GetStatemenets()
    {
        return m_programStatements;
    }

private:
    DeclarationBlock *m_declarationBlock;
    Statement        *m_programStatements = nullptr;
};
