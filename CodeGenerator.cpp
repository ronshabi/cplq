#include "CodeGenerator.hpp"

#include "Module.hpp"

CodeGenerator::CodeGenerator(Module &mod)
    : m_module(mod)
{
}

void
CodeGenerator::codegen(Program *node)
{
    node->get_statemenets()->accept_code_generator(*this);
    emit_line("HALT");
}

void
CodeGenerator::codegen(DeclarationBlock *node)
{
    // Nothing?
}

void
CodeGenerator::codegen(StatementList *node)
{
    for (auto const& stmt : node->get_statements()) {
        stmt->accept_code_generator(*this);
    }
}

void
CodeGenerator::codegen(AssignStatement *node)
{
    // const auto type = node->get_expression()->get_type(symbol_table());

    // if (type == NumberType::FLOAT) {

    // }
}

void
CodeGenerator::codegen(InputStatement *node)
{
}

void
CodeGenerator::codegen(OutputStatement *node)
{
}

void
CodeGenerator::codegen(IfStatement *node)
{
}

void
CodeGenerator::codegen(SwitchStatement *node)
{
}

void
CodeGenerator::codegen(WhileStatement *node)
{
}

void
CodeGenerator::codegen(CastExpression *node)
{
    const auto cast_type = node->get_type(symbol_table());

    if (cast_type == NumberType::INT) {
        // Cast to int -> RTOI
        emit_line("RTOI", node->get_register(), node->get_expression()->get_register());
    } else {
        // Cast to real -> ITOR
        emit_line("ITOR", node->get_register(), node->get_expression()->get_register());
    }
}

void
CodeGenerator::codegen(NotExpression *node)
{
}

void
CodeGenerator::codegen(BinaryBoolExpression *node)
{
}

void
CodeGenerator::codegen(BinaryExpression *node)
{
}

void
CodeGenerator::codegen(IdExpression *node)
{
    // const auto val = symbol_table().get(*node->get_id());
    // if (val->value.type == NumberType::INT) {
        
        // node-> = std::to_string(val->value.i);
    // } else {
    //     node->m_code.code = std::to_string(val->value.d);
    // }

    // std::cout << "IdExpr CodeGen: " << node->m_code.code;
}

void CodeGenerator::emit_line(std::string const& str) {
    m_module.add_assembly_line(str);
}

void CodeGenerator::emit_line(std::string const& opcode, Register r) {
    m_module.add_assembly_line(opcode + " " + r.name());
}

void CodeGenerator::emit_line(std::string const& opcode, Register r1, Register r2) {
    m_module.add_assembly_line(opcode + " " + r1.name() + " " + r2.name());
}

SymbolTable& CodeGenerator::symbol_table() {
    return m_module.get_symbol_table();
}
