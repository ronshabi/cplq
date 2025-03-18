#include "Ast.hpp"

#include "CodeGenerator.hpp"
#include "SemanticAnalyzer.hpp"
#include <unordered_set>

void
AstNode::dump(unsigned indent) const
{
    for (unsigned i = 0; i < indent; ++i)
    {
        std::cout << ' ';
    }
}

void
DeclarationBlock::add_declaration(Declaration *declaration)
{
    m_declarations.emplace_back(declaration);
}

void
DeclarationBlock::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "DeclBlock<" << m_declarations.size() << ">\n";
    for (const auto &decl : m_declarations)
    {
        decl->dump(indent + 4);
    }
}

void
DeclarationBlock::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
DeclarationBlock::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
BinaryBoolExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "BinaryBoolExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void
BinaryBoolExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
BinaryBoolExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
Declaration::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Decl<" << number_type_to_string(m_type) << "> "
              << vector_to_string(m_ids) << "\n";
}

void
Declaration::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    UNUSED(sema);
}

void
Declaration::accept_code_generator(CodeGenerator &cg)
{
    UNUSED(cg);
}

void
NotExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "NotExp\n";
    m_exp->dump(indent + 4);
}

void
NotExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
NotExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
NumberExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    if (m_number.type == NumberType::FLOAT)
    {
        std::cout << "Number<Float>(" << m_number.d << ")\n";
    }
    else
    {
        std::cout << "Number<Int>(" << m_number.i << ")\n";
    }
}

void
NumberExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    UNUSED(sema);
}

void
NumberExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
CastExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Cast<" << number_type_to_string(m_cast) << ">\n";
    m_expression->dump(indent + 4);
}

void
CastExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
CastExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
IdExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Id<" << *m_id << ">\n";
}

void
IdExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
IdExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

NumberType
IdExpression::get_type(SymbolTable const &symbol_table) const
{
    auto const sym = symbol_table.get(*m_id);
    if (sym.has_value())
    {
        return sym->value.type;
    }
    else
    {
        return NumberType::INT; // Default value. If the number has no value
                                // It will be caught in the semantic analyzer
                                // before this
    }
}

void
BinaryExpression::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "BinaryExpression<" << operator_to_string(m_op) << ">\n";
    m_exp1->dump(indent + 4);
    m_exp2->dump(indent + 4);
}

void
BinaryExpression::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
BinaryExpression::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

NumberType
BinaryExpression::get_type(SymbolTable const &symbol_table) const
{
    auto const exp1_type = m_exp1->get_type(symbol_table);
    auto const exp2_type = m_exp2->get_type(symbol_table);
    if (exp1_type == NumberType::FLOAT || exp2_type == NumberType::FLOAT)
    {
        return NumberType::FLOAT;
    }

    return NumberType::INT;
}

void
AssignStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Assign<" << *m_id << ">\n";
    m_exp->dump(indent + 4);
}

void
AssignStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
AssignStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
OutputStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Output\n";
    m_exp->dump(indent + 4);
}

void
OutputStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
OutputStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
InputStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Input<" << *m_id << ">\n";
}

void
InputStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
InputStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
IfStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "If\n";
    m_condition_exp->dump(indent + 4);
    m_then_branch->dump(indent + 4);
    m_else_branch->dump(indent + 4);
}

void
IfStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
IfStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
WhileStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "While\n";
    m_condition_exp->dump(indent + 4);
    m_body_statement->dump(indent + 4);
}

void
WhileStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
WhileStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
BreakStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Break\n";
}

void
BreakStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    // nop
    UNUSED(sema);
}

void
BreakStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
StatementList::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "StatementList\n";
    for (auto const &stmt : m_statements)
    {
        stmt->dump(indent + 4);
    }
}

void
StatementList::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
StatementList::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
SwitchStatement::dump(unsigned indent) const
{
    AstNode::dump(indent);
    std::cout << "Switch\n";
    m_exp->dump(indent + 4);
    for (auto const &case_pair : m_cases)
    {
        NumberExpression x(case_pair.first);
        x.dump(indent + 4);
        (case_pair.second)->dump(indent + 8);
    }

    AstNode::dump(indent + 4);
    std::cout << "Default\n";
    m_default_case->dump(indent + 8);
}

void
SwitchStatement::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
SwitchStatement::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}

void
Program::dump(unsigned indent) const
{
    AstNode::dump(indent);
    m_declaration_block->dump(0);
    m_program_statements->dump(0);
}

void
Program::accept_semantic_analyzer(SemanticAnalyzer &sema)
{
    sema.analyze(this);
}

void
Program::accept_code_generator(CodeGenerator &cg)
{
    cg.codegen(this);
}
