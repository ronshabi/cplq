#include "SemanticAnalyzer.hpp"

#include "DebugStream.hpp"
#include "Module.hpp"

void SemanticAnalyzer::analyze(Program *node)
{
    node->get_declaration_block()->accept_semantic_analyzer(*this);
    node->get_statemenets()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(DeclarationBlock *node)
{
    auto const decls = node->get_declarations();
    for (auto const decl : decls)
    {
        m_module.set_current_position(decl->get_line_number());

        auto const decl_ids = decl->get_ids();
        for (auto const id : decl_ids)
        {
            auto const id_optional = m_symbol_table.get(*id);

            // ID already exists in the symbol table. This is an Error.
            if (id_optional.has_value())
            {
                // Give detailed error based on its type
                m_module.emit_error("repeated declaration of symbol '%s'",
                                    id->c_str());
            }
            // ID doesn't exist in the symbol table, so add it.
            else
            {
                m_symbol_table.add(
                    *id, Symbol(decl->get_type(), decl->get_line_number()));

                    DBG("added symbol '" << *id << "' to symbol table");
            }
        }
    }
}

void SemanticAnalyzer::analyze(StatementList *node) {
    for (auto const& stmt : node->get_statements()) {
        stmt->accept_semantic_analyzer(*this);
    }
}

void SemanticAnalyzer::analyze(AssignStatement *node) {
    // Check that the ID is in the symbol table
    auto const id_sym = m_symbol_table.get(*node->get_id());
    m_module.set_current_position(node->get_line_number());

    if (!id_sym.has_value()) {
        m_module.emit_error("unknown indentifier '%s' in assign statement", node->get_id()->c_str());
    }

    // Check the expression
    node->get_expression()->accept_semantic_analyzer(*this);

    // Check that the expression is not a boolean expression
    if (node->get_expression()->is_bool_expr()) {
        m_module.emit_error("a boolean expression cannot be an asignee");
        return;
    }

    // Typechecker: Check the type of the expression, if it's a float, and we're an int, deny
    if (node->get_expression()->get_type(m_symbol_table) == NumberType::FLOAT && id_sym->value.type == NumberType::INT) {
        m_module.emit_error("type mismatch: can't assign a float to an int variable");
        return;
    }
}

void SemanticAnalyzer::analyze(InputStatement *node) {
    // Check the indentifier is in the symbol table
    auto const id = node->get_id();
    auto const sym = m_symbol_table.get(*id);
    if (!sym.has_value()) {
        m_module.set_current_position(node->get_line_number());
        m_module.emit_error("unknown indentifier '%s' in input function", id->c_str());
    }
}

void SemanticAnalyzer::analyze(OutputStatement *node) {
    node->get_expression()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(IfStatement *node) {
    node->get_condition_exp()->accept_semantic_analyzer(*this);
    node->get_then_branch()->accept_semantic_analyzer(*this);
    node->get_else_branch()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(BinaryExpression *node) {
    node->get_lhs_expr()->accept_semantic_analyzer(*this);
    node->get_rhs_expr()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(BoolExpression *node) {
    DBG("TODO");
}

void SemanticAnalyzer::analyze(IdExpression *node) {
    // Check the ID is in the symbol table
    auto const sym = m_symbol_table.get(*node->get_id());
    if (!sym.has_value()) {
        m_module.set_current_position(node->get_line_number());
        m_module.emit_error("unknown variable '%s'", node->get_id()->c_str());
    }
}

void SemanticAnalyzer::analyze(SwitchStatement *node) {
    DBG("TODO");
}

void SemanticAnalyzer::analyze(WhileStatement *node) {
    node->get_condition_expression()->accept_semantic_analyzer(*this);
    node->get_body_statement()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(CastExpression *node) {
    DBG("TODO");
    
}

void SemanticAnalyzer::analyze(NotExpression *node) {
    DBG("TODO");
    
}

void SemanticAnalyzer::analyze(BinaryBoolExpression *node) {
    node->get_lhs_expr()->accept_semantic_analyzer(*this);
    node->get_rhs_expr()->accept_semantic_analyzer(*this);
}

