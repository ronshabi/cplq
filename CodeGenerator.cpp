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
    // Jmpz to the else branch
    auto const after_condition = current_asm_line_number();
    emit_filler("if: after condition, jmpz should be inserted");
    
    node->get_then_branch()->accept_code_generator(*this);
    
    // Jmp to after the IF
    
    auto const after_then = current_asm_line_number();
    emit_filler("if: after then, a jump to the rest of the code should be inserted");


    node->get_else_branch()->accept_code_generator(*this);
    auto const after_if = current_asm_line_number();

    auto &asm_vec = m_module.m_assembly;

    printf("IFSTMT JUMPZ @ %d\n", after_condition + 1);
    asm_vec[after_condition] =
                   "JMPZ " + std::to_string(after_then + 2) + " " +
                       condition_register.name();
    asm_vec[after_then] =
                   "JUMP " + std::to_string(after_if + 1);
}

void
CodeGenerator::codegen(SwitchStatement *node)
{
}

void
CodeGenerator::codegen(WhileStatement *node)
{
    // Call the codegen on the condition
    auto const cond_check_line = current_asm_line_number();
    node->get_condition_expression()->accept_code_generator(*this);

    // Check the return code, if it is zero, we should resume to normal code

    auto const after_checking_cond = current_asm_line_number();
    emit_filler("while after condition");
    // BUT WHERE IS THE NORMAL CODE?
    node->get_body_statement()->accept_code_generator(*this);

    // Jump to the condition check once again
    emit_line("JUMP " + std::to_string(cond_check_line + 1));

    // This is where the normal code resumes, we need to document this for break
    // and loop exit
    m_break_jump_dest = current_asm_line_number();

    auto &asm_vec = m_module.m_assembly;

    std::string jmpz_exit_loop =
        "JMPZ " + std::to_string(m_break_jump_dest + 1) + " " +
        node->get_condition_expression()->get_register().name();

    asm_vec[after_checking_cond] = jmpz_exit_loop;
    backpatch_break_statements();
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
    node->get_expression()->accept_code_generator(*this);
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
        // We need to evaluate LHS first. If LHS=0, we can skip RHS.
        // How can we skip RHS?
        //  - If LHS=0 => continue to regular code
        //  - Else (LHS=1) => jump to evaluate RHS.
        //
        //      ~> Therefore, the LHS evaluation should come first.

        // Set our register to zero
        emit_line("IASN", my_register, 0);

        node->get_lhs_expr()->accept_code_generator(*this);

        // If LHS=0 => Jump to regular code
        auto const after_lhs_eval = current_asm_line_number();

        // If LHS=1 => Continue to evaluating RHS
        node->get_rhs_expr()->accept_code_generator(*this);

        // If RHS=1 => Set our register to one
        emit_line("IEQL", my_register, node->get_rhs_expr()->get_register(), 1);

        // The normal code resumes here
        auto const normal_code_resumes = current_asm_line_number();

        std::string const jmpz_instr =
            "JMPZ " + std::to_string(normal_code_resumes + 2) + " " +
            node->get_lhs_expr()->get_register().name();

        asm_vec.insert(asm_vec.begin() + after_lhs_eval, jmpz_instr);
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
        emit_filler("Boolean OR: RHS Eval Line");
        node->get_rhs_expr()->accept_code_generator(*this);
        auto const lhs_eval_line = current_asm_line_number();
        // emit_filler("Boolean OR: LHS Eval Line");
        node->get_lhs_expr()->accept_code_generator(*this);

        // If LHS=0 => jump to RHS evaluation
        emit_line("JMPZ " + std::to_string(rhs_eval_line + 1),
                  node->get_lhs_expr()->get_register());

        // Insert the jump to LHS, right before RHS eval code
        asm_vec.at(rhs_eval_line) = "JUMP " + std::to_string(lhs_eval_line + 1);
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
        bool use_int        = true;
        bool should_convert = false;
        // If one of the operands is a float, convert the other one to int and
        // use
        // the float variation
        if (lhs_type != rhs_type)
        {
            should_convert = true;
        }

        if (should_convert && lhs_type == NumberType::INT)
        {
            Register cvt;
            use_int = false;
            emit_line("ITOR", cvt, lhs_register);
            lhs_register = cvt;
        }

        if (should_convert && rhs_type == NumberType::INT)
        {
            Register cvt;
            use_int = false;
            emit_line("ITOR", cvt, rhs_register);
            rhs_register = cvt;
        }

        if (lhs_type == NumberType::FLOAT)
        {
            use_int = false;
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
    bool use_int        = true;
    bool should_convert = false;
    // If one of the operands is a float, convert the other one to int and
    // use
    // the float variation
    if (lhs_type != rhs_type)
    {
        should_convert = true;
    }

    if (should_convert && lhs_type == NumberType::INT)
    {
        Register cvt;
        use_int = false;
        emit_line("ITOR", cvt, lhs_register);
        lhs_register = cvt;
    }

    if (should_convert && rhs_type == NumberType::INT)
    {
        Register cvt;
        use_int = false;
        emit_line("ITOR", cvt, rhs_register);
        rhs_register = cvt;
    }

    if (lhs_type == NumberType::FLOAT)
    {
        use_int = false;
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
    UNUSED(node);
    emit_line("JUMP "); // To be backpatched
    m_break_requests.push_back(current_asm_line_number());
}

void CodeGenerator::emit_filler(std::string const& comment) {
    m_module.m_assembly.push_back("FILLER: " + comment);
}

void
CodeGenerator::emit_line(std::string const &opcode)
{
    m_module.m_assembly.push_back(opcode);
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r)
{
    m_module.m_assembly.push_back(opcode + " " + r.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2)
{
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val)
{
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::to_string(val));
}
void
CodeGenerator::emit_line(std::string const &opcode, Register r, int val1,
                         int val2)
{
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::to_string(val1) + " " +
                                  std::to_string(val2));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         int val)
{
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name() +
                                  " " + std::to_string(val));
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r1, Register r2,
                         Register r3)
{
    m_module.m_assembly.push_back(opcode + " " + r1.name() + " " + r2.name() +
                                  " " + r3.name());
}

void
CodeGenerator::emit_line(std::string const &opcode, Register r, double val)
{
    std::array<char, 128> buf{};
    std::snprintf(buf.data(), buf.size(), "%.2f", val);
    m_module.m_assembly.push_back(opcode + " " + r.name() + " " +
                                  std::string(buf.data()));
}

SymbolTable &
CodeGenerator::symbol_table()
{
    return m_module.get_symbol_table();
}

void
CodeGenerator::backpatch_break_statements()
{
    auto &asm_vec = m_module.m_assembly;

    for (unsigned const br : m_break_requests)
    {
        asm_vec[br].append(std::to_string(m_break_jump_dest + 1));
    }

    m_break_requests.clear();
}

unsigned
CodeGenerator::current_asm_line_number() const
{
    return m_module.m_assembly.size();
}
