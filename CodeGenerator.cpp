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

    Register convert_result;

    if (id_symbol->value.type == NumberType::INT)
    {
        convert_result = rhs_expr->get_register();
        emit_line("IASN", id_symbol->reg, convert_result);
    }
    else
    {
        // Check RHS expression type, convert it to float if needed
        if (rhs_type == NumberType::INT)
        {
            emit_line("ITOR", convert_result, rhs_expr->get_register());
        }
        else
        {
            convert_result = rhs_expr->get_register();
        }

        emit_line("RASN", id_symbol->reg, convert_result);
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
    node->get_condition_exp()->accept_code_generator(*this);
    auto const condition_register = node->get_condition_exp()->get_register();
    auto const before_then_branch = current_asm_line_number();
    // Jmpz to the else branch
    node->get_then_branch()->accept_code_generator(*this);
    // Jmp to after the IF
    auto const before_else_branch = current_asm_line_number();
    node->get_else_branch()->accept_code_generator(*this);
    auto const after_if = current_asm_line_number();

    auto &asm_vec = m_module.m_assembly;

    asm_vec.insert(asm_vec.begin() + before_then_branch,
                   "JMPZ " + std::to_string(before_else_branch + 3) + " " +
                       condition_register.name());
    asm_vec.insert(asm_vec.begin() + before_else_branch + 1,
                   "JMP " + std::to_string(after_if + 3));
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
    auto const my_register = node->get_register();
    auto const my_operator = node->get_operator();
    auto      &asm_vec     = m_module.m_assembly;

    if (my_operator == Operator::AND)
    {
        // Short-circuting AND
        // If the LHS expression is false, we can skip RHS
        // So evaluate (codegen) for LHS first
        node->get_lhs_expr()->accept_code_generator(*this);
        // Here, if LHS=0, we need to SKIP the RHS evaluation
        // Let's write the assembly line here to later insert the JMPZ
        auto const jmpz_line = current_asm_line_number();
        // Codegen for RHS to evaluate it.
        node->get_rhs_expr()->accept_code_generator(*this);
        // Now we need to ask if RHS is 1, and if it is 1, then the whole
        // binary bool expression is one.
        emit_line("IEQL", my_register, node->get_rhs_expr()->get_register(), 1);
        // This is where we go to if the LHS expression was 0
        auto const lhs_zero_goto = current_asm_line_number() + 1;
        // So let's generate the code for this
        std::string const jmpz_instr =
            "JMPZ " + std::to_string(lhs_zero_goto) + " " +
            node->get_lhs_expr()->get_register().name();
        asm_vec.insert(asm_vec.begin() + jmpz_line, jmpz_instr);
    }
    else if (my_operator == Operator::OR)
    {
        // Short-circuting OR
        // We need to evaluate LHS first. If LHS=1, we can skip RHS.
        // How can we skip RHS?
        //  - If LHS=0 => jump to the evalutation of RHS,
        //  - Else (LHS=1) => continue to normal code.
        //
        //    ~> Therefore, normal code should come RIGHT AFTER LHS evaluation

        // Generate RHS evalutation first, and remember to insert the jump to
        // the LHS eval before
        auto const rhs_eval_line = current_asm_line_number();
        node->get_rhs_expr()->accept_code_generator(*this);
        auto const lhs_eval_line = current_asm_line_number();
        node->get_lhs_expr()->accept_code_generator(*this);

        // If LHS=0 => jump to RHS evaluation
        emit_line("JMPZ " + std::to_string(rhs_eval_line + 1),
                  node->get_lhs_expr()->get_register());

        // Insert the jump to LHS, right before RHS eval code
        asm_vec.insert(asm_vec.begin() + rhs_eval_line,
                       "JUMP " + std::to_string(lhs_eval_line + 1));
    }
    else
    {
        node->get_lhs_expr()->accept_code_generator(*this);
        node->get_rhs_expr()->accept_code_generator(*this);

        auto       lhs_register = node->get_lhs_expr()->get_register();
        auto       rhs_register = node->get_rhs_expr()->get_register();
        auto const lhs_type = node->get_lhs_expr()->get_type(symbol_table());
        auto const rhs_type = node->get_rhs_expr()->get_type(symbol_table());

        // If both operands are ints, use the int variation by default.
        bool use_int = true;

        // If one of the operands is a float, convert the other one to int and
        // use
        // the float variation
        Register cvt;
        if (lhs_type == NumberType::INT && rhs_type == NumberType::FLOAT)
        {
            use_int = false;
            emit_line("ITOR", cvt, lhs_register);
            lhs_register = cvt;
        }

        if (lhs_type == NumberType::FLOAT && rhs_type == NumberType::INT)
        {
            use_int = false;
            emit_line("ITOR", cvt, rhs_register);
            rhs_register = cvt;
        }

        std::string opcode;
        opcode.push_back(use_int ? 'I' : 'R'); // Choose the right prefix

        bool invert_logical_condition = false; // If we need to use GTE, LTE ops

        // Now we know what variation we have to use, and we have both registers
        // in the proper types
        switch (node->get_operator())
        {
        case Operator::EQ:
            opcode.append("EQL");
            break;
        case Operator::NEQ:
            opcode.append("NQL");
            break;
        case Operator::LT:
            opcode.append("LSS");
            break;
        case Operator::GT:
            opcode.append("GRT");
            break;
        case Operator::LTE:
            opcode.append("GRT");
            invert_logical_condition = true;
            break;
        case Operator::GTE:
            opcode.append("LSS");
            invert_logical_condition = true;
            break;
        default:
            m_module.emit_fatal_error(
                "invalid operator on boolean binary expression");
        }

        // Now let's generate the code
        emit_line(opcode, my_register, lhs_register, rhs_register);
        // If we need to invert the logical condition to get a "*-or-equals"
        // inst let's emit a NOT also
        if (invert_logical_condition)
        {
            emit_line("INQL", my_register, my_register, 1);
        }

        // That's it!
    }
}

void
CodeGenerator::codegen(BinaryExpression *node)
{
    node->get_lhs_expr()->accept_code_generator(*this);
    node->get_rhs_expr()->accept_code_generator(*this);
    auto const my_register  = node->get_register();
    auto       lhs_register = node->get_lhs_expr()->get_register();
    auto       rhs_register = node->get_rhs_expr()->get_register();

    auto const lhs_type = node->get_lhs_expr()->get_type(symbol_table());
    auto const rhs_type = node->get_rhs_expr()->get_type(symbol_table());

    // If both types are ints:
    //      - use the I-variation of the instruction.
    //      - this will be the default
    // Else (meaning one of the types are floats):
    //      - convert the other one to float
    //      - use the R-variation of the instruction
    bool use_int = true;

    // Check if one of the operands are floats, and convert if needed
    Register cvt;
    if (lhs_type == NumberType::INT && rhs_type == NumberType::FLOAT)
    {
        use_int = false;
        emit_line("ITOR", cvt, lhs_register);
        lhs_register = cvt;
    }

    if (lhs_type == NumberType::FLOAT && rhs_type == NumberType::INT)
    {
        use_int = false;
        emit_line("ITOR", cvt, rhs_register);
        rhs_register = cvt;
    }

    // The opcode naming is quite redundant, so let's not repeat ourselves
    std::string opcode;
    opcode.push_back(use_int ? 'I' : 'R'); // Choose the right prefix

    // Choose the instruction based on the operator

    switch (node->get_operator())
    {
    case Operator::PLUS:
        opcode.append("ADD");
        break;
    case Operator::MINUS:
        opcode.append("SUB");
        break;
    case Operator::MUL:
        opcode.append("MLT");
        break;
    case Operator::DIV:
        opcode.append("DIV");
        break;
    default:
        m_module.emit_fatal_error("invalid operator in binary expression");
    }

    // Generate the code
    emit_line(opcode, my_register, lhs_register, rhs_register);
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
    m_module.m_assembly.push_back(opcode);
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r)
{
    DBG(opcode << " " << r.name());
    m_module.m_assembly.push_back(opcode + " " + r.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name());
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val)
{
    DBG(opcode << " " << r.name() << ", " << val);
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::to_string(val));
}
void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val1,
                         int val2)
{
    DBG(opcode << " " << r.name() << ", " << val1 << ", " << val2);
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::to_string(val1) + " " +
                                  std::to_string(val2));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         int val)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name() << ", " << val);
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name() +
                                  " " + std::to_string(val));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         Register r3)
{
    DBG(opcode << " " << r1.name() << ", " << r2.name() << ", " << r3.name());
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name() +
                                  " " + r3.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, double val)
{
    std::array<char, 128> buf{};
    std::snprintf(buf.data(), buf.size(), "%.2f", val);
    DBG(opcode << " " << r.name() << ", " << buf.data());
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::string(buf.data()));
}

SymbolTable &
CodeGenerator::symbol_table()
{
    return m_module.get_symbol_table();
}

unsigned
CodeGenerator::current_asm_line_number() const
{
    return m_module.m_assembly.size();
}
