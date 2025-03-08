#include "SemanticAnalyzer.hpp"

#include "Module.hpp"

void SemanticAnalyzer::analyze(Program *node)
{
    node->get_declaration_block()->accept_semantic_analyzer(*this);
    node->get_statemenets()->accept_semantic_analyzer(*this);
}

void SemanticAnalyzer::analyze(DeclarationBlock *db)
{
    auto const decls = db->get_declarations();
    for (auto const decl : decls)
    {
        decl->dump(2);
        m_module.set_current_position(decl->get_line_number());

        auto const decl_ids = decl->get_ids();
        for (auto const id : decl_ids)
        {
            auto const id_optional = m_symbol_table.get(*id);

            // ID already exists in the symbol table. This is an Error.
            if (id_optional.has_value())
            {
                // Give detailed error based on its type
                m_module.emit_error("Repeated declaration of symbol '%s'",
                                    id->c_str());
            }
            // ID doesn't exist in the symbol table, so add it.
            else
            {
                m_symbol_table.add(
                    *id, Symbol(decl->get_type(), decl->get_line_number()));
            }
        }

        // Try to add to symbol table, else - ERROR!
    }
}
