#include "CodeGenerator.hpp"

#include "Module.hpp"
#include <array>
#include <string>

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
    UNUSED(node);
}

void
CodeGenerator::codegen(StatementList *node)
{
    for (auto const &stmt : node->get_statements())
    {
        stmt->accept_code_generator(*this);
    }
}

void
CodeGenerator::codegen(AssignStatement *node)
{
    // Generate code for the RHS expression
    // We do this first because we first need to evaluate RHS when assigning!
    node->get_expression()->accept_code_generator(*this);

    // Get ID of symbol
    auto const id_symbol = symbol_table().get(*node->get_id());

    auto const rhs_expr = node->get_expression();
    auto const rhs_type = rhs_expr->get_type(symbol_table());

    if (id_symbol->value.type == NumberType::INT)
    {
        // Check RHS expression type, convert it to int if needed
        emit_line("IASN", id_symbol->reg,
                  node->get_expression()->get_register());
    }
    else
    {
        Register convert_result;

        // Check RHS expression type, convert it to float if needed
        if (rhs_type == NumberType::INT) {
            emit_line("ITOR", convert_result, rhs_expr->get_register());
        } else {
            convert_result = rhs_expr->get_register();
        }

        emit_line("RASN", id_symbol->reg,
                  convert_result);
    }
}

void
CodeGenerator::codegen(InputStatement *node)
{
    // Get the symbol properties to know which opcode to use
    auto const sym  = symbol_table().get(*node->get_id());
    auto const type = sym->value.type;

    if (type == NumberType::INT)
    {
        emit_line("IINP", sym->reg);
    }
    else
    {
        emit_line("RINP", sym->reg);
    }
}

void
CodeGenerator::codegen(OutputStatement *node)
{
    node->get_expression()->accept_code_generator(*this);
    auto const type = node->get_expression()->get_type(symbol_table());
    if (type == NumberType::INT)
    {
        emit_line("IPRT", node->get_expression()->get_register());
    }
    else
    {
        emit_line("RPRT", node->get_expression()->get_register());
    }
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
    node->get_expression()->accept_code_generator(*this);
    const auto cast_type = node->get_type(symbol_table());

    if (cast_type == NumberType::INT)
    {
        // Cast to int -> RTOI
        emit_line("RTOI", node->get_register(),
                  node->get_expression()->get_register());
    }
    else
    {
        // Cast to real -> ITOR
        emit_line("ITOR", node->get_register(),
                  node->get_expression()->get_register());
    }
}

void
CodeGenerator::codegen(NotExpression *node)
{
    node->accept_code_generator(*this);
    emit_line("INQL", node->get_register(),
              node->get_expression()->get_register(), 1);
}

void
CodeGenerator::codegen(BinaryBoolExpression *node)
{
}

void
CodeGenerator::codegen(BinaryExpression *node)
{
    node->get_lhs_expr()->accept_code_generator(*this);
    node->get_rhs_expr()->accept_code_generator(*this);
    const auto my_register  = node->get_register();
    const auto lhs_register = node->get_lhs_expr()->get_register();
    const auto rhs_register = node->get_rhs_expr()->get_register();
    const auto type         = node->get_type(symbol_table());

    switch (node->get_operator())
    {
    case Operator::PLUS:
        if (type == NumberType::INT)
        {
            emit_line("IADD", my_register, lhs_register, rhs_register);
        }
        else
        {
            emit_line("RADD", my_register, lhs_register, rhs_register);
        }
        break;
    case Operator::MINUS:
        if (type == NumberType::INT)
        {
            emit_line("ISUB", my_register, lhs_register, rhs_register);
        }
        else
        {
            emit_line("RSUB", my_register, lhs_register, rhs_register);
        }
        break;
    case Operator::MUL:
        if (type == NumberType::INT)
        {
            emit_line("IMLT", my_register, lhs_register, rhs_register);
        }
        else
        {
            emit_line("RMLT", my_register, lhs_register, rhs_register);
        }
        break;
    case Operator::DIV:
        if (type == NumberType::INT)
        {
            emit_line("IDIV", my_register, lhs_register, rhs_register);
        }
        else
        {
            emit_line("RDIV", my_register, lhs_register, rhs_register);
        }
        break;
    default:
        m_module.emit_fatal_error("invalid operator in binary expression");
    }
}

void
CodeGenerator::codegen(IdExpression *node)
{
    // I want to SET the ID register to the symbol register, to sync them.
    const auto sym   = symbol_table().get(*node->get_id());
    node->m_register = sym->reg;

    // Now we can continue with no worries the ID will not refer to the symbol
}

void
CodeGenerator::codegen(NumberExpression *node)
{
    const auto type = node->get_type(symbol_table());
    if (type == NumberType::INT)
    {
        emit_line("IASN", node->get_register(), node->get_number().i);
    }
    else
    {
        emit_line("RASN", node->get_register(), node->get_number().d);
    }
}

void
CodeGenerator::codegen(BreakStatement *node)
{
}

void
CodeGenerator::emit_line(std::string const &opcode)
{
    DBG(opcode);
    m_module.add_assembly_line(opcode);
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r)
{
    DBG(opcode << " " << r.name());
    m_module.add_assembly_line(opcode + " " + r.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name());
    m_module.add_assembly_line(opcode + " " + r1.name() + " " + r2.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val)
{
    DBG(opcode << " " << r.name() << ", " << val);
    m_module.add_assembly_line(opcode + " " + r.name() + " " +
                               std::to_string(val));
}
void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val1,
                         int val2)
{
    DBG(opcode << " " << r.name() << ", " << val1 << ", " << val2);
    m_module.add_assembly_line(opcode + " " + r.name() + " " +
                               std::to_string(val1) + " " +
                               std::to_string(val2));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         int val)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name() << ", " << val);
    m_module.add_assembly_line(opcode + " " + r1.name() + " " + r2.name() +
                               " " + std::to_string(val));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         Register r3)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name() << ", " << r3.name());
    m_module.add_assembly_line(opcode + " " + r1.name() + " " + r2.name() +
                               " " + r3.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, double val)
{
    std::array<char, 128> buf{};
    std::snprintf(buf.data(), buf.size(), "%.2f", val);
    DBG(opcode << " " << r.name() << ", " << buf.data());
    m_module.add_assembly_line(opcode + " " + r.name() + " " +
                               std::string(buf.data()));
}

SymbolTable &
CodeGenerator::symbol_table()
{
    return m_module.get_symbol_table();
}
