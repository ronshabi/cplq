//===-- CodeGenerator.cpp -------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#include "CodeGenerator.hpp"

#include "Module.hpp"
#include <array>
#include <string>

CodeGenerator::CodeGenerator(Module &mod)
    : m_module(mod)
{
}

void
CodeGenerator::Codegen(Program *node)
{
    node->GetStatemenets()->AcceptCodeGenerator(*this);
    EmitLine("HALT");
}

void
CodeGenerator::Codegen(DeclarationBlock *node)
{
    UNUSED(node);
}

void
CodeGenerator::Codegen(StatementList *node)
{
    for (auto const &stmt : node->GetStatements())
    {
        stmt->AcceptCodeGenerator(*this);
    }
}

void
CodeGenerator::Codegen(AssignStatement *node)
{
    // Generate code for the RHS expression
    // We do this first because we first need to evaluate RHS when assigning!
    node->GetExpr()->AcceptCodeGenerator(*this);

    // Get ID of symbol
    auto const idSymbol = GetSymbolTable().Get(*node->GetID());

    auto const rhsExpr = node->GetExpr();
    auto const rhsType = rhsExpr->GetType(GetSymbolTable());

    Register convertResult;

    if (idSymbol->value.type == NumberType::INT)
    {
        convertResult = rhsExpr->GetRegister();
        EmitLine("IASN", idSymbol->reg, convertResult);
    }
    else
    {
        // Check RHS expression type, convert it to float if needed
        if (rhsType == NumberType::INT)
        {
            EmitLine("ITOR", convertResult, rhsExpr->GetRegister());
        }
        else
        {
            convertResult = rhsExpr->GetRegister();
        }

        EmitLine("RASN", idSymbol->reg, convertResult);
    }
}

void
CodeGenerator::Codegen(InputStatement *node)
{
    // Get the symbol properties to know which opcode to use
    auto const sym  = GetSymbolTable().Get(*node->GetID());
    auto const type = sym->value.type;

    if (type == NumberType::INT)
    {
        EmitLine("IINP", sym->reg);
    }
    else
    {
        EmitLine("RINP", sym->reg);
    }

    node->m_register = sym->reg;
}

void
CodeGenerator::Codegen(OutputStatement *node)
{
    node->GetExpr()->AcceptCodeGenerator(*this);
    auto const type = node->GetExpr()->GetType(GetSymbolTable());
    if (type == NumberType::INT)
    {
        EmitLine("IPRT", node->GetExpr()->GetRegister());
    }
    else
    {
        EmitLine("RPRT", node->GetExpr()->GetRegister());
    }
}

void
CodeGenerator::Codegen(IfStatement *node)
{
    // The algorithm of if statement generation

    //              [Condition code]
    //              If condition_register==0: jump to L1 (else-block)
    //              [Then-block code]
    //              Jump to L2 (skip else-block, exit the if)
    //      L1:     [Else-block code]
    //      L2:     [... Rest of code ...]

    auto const conditionRegister = node->GetConditionExpr()->GetRegister();

    node->GetConditionExpr()->AcceptCodeGenerator(*this);

    auto const jzl1Filler = EmitFiller("If: Jump to else block");

    node->GetThenBranch()->AcceptCodeGenerator(*this);

    auto const jtl2Filler = EmitFiller("If: Skip else block");

    auto const L1 = GetCurrentAsmLineNumber();

    node->GetElseBranch()->AcceptCodeGenerator(*this);

    auto const L2 = GetCurrentAsmLineNumber();

    // Backpatch
    PatchJmpz(jzl1Filler, L1 + 1, conditionRegister);
    PatchJump(jtl2Filler, L2 + 1);
}

void
CodeGenerator::Codegen(SwitchStatement *node)
{
    // The algorithm of switch-statement code generation.

    // Let m be the number of cases.
    // Let RExpr be the expression result register.
    // Let RCmp be a temporary register for case comparison.
    // Let Nx be the case num register.

    //                  [checked expression code]
    //      LC1:        Set RCmp = (RExpr == Nx)
    //                  If RCmp == 0: Jump to LC2
    //                  [case 1 code]
    //                  Jump to the end of the switch
    //      ...
    //
    //      LCm:        Set RCmp = (RExpr == Nm)
    //                  If RCmp == 0: Jump to LD (D for default)
    //                  [case m code]
    //                  Jump to the end of the switch
    //
    //      LD:         [default case code]
    //
    //      LExit:      [... rest of the code after the switch ...]

    auto const expr = node->GetExpr();

    auto       rcmp  = Register();
    auto const cases = node->GetCases(); // First - the case number
                                         // Second - the case statement list
    std::vector<unsigned> caseLabels{};
    std::vector<unsigned> caseBackpatchLines{};
    std::vector<unsigned> caseImplicitJumpToNextBackpatchLines{};

    expr->AcceptCodeGenerator(*this);
    auto const rexpr = node->GetExpr()->GetRegister();

    // Generate code for regular cases
    for (std::size_t i = 0; i < cases.size(); ++i)
    {
        auto const caseNumber = cases.at(i).first.i;
        auto const caseCode   = cases.at(i).second;

        caseLabels.push_back(GetCurrentAsmLineNumber());
        EmitLine("IEQL", rcmp, rexpr, caseNumber);

        auto const lcnext = EmitFiller("Switch: if rcmp==0, jump to next case");
        caseBackpatchLines.push_back(lcnext);

        caseCode->AcceptCodeGenerator(*this);

        auto const jtImplicitNext =
            EmitFiller("switch: jump implicit to next");
        caseImplicitJumpToNextBackpatchLines.push_back(jtImplicitNext);
    }

    // Generate code for the default case
    auto const LD = GetCurrentAsmLineNumber();
    node->GetDefaultCase()->AcceptCodeGenerator(*this);

    auto const LExit = GetCurrentAsmLineNumber();

    // ---------------
    // Backpatch
    // ---------------
    // Don't forget the breaks
    m_breakJumpDest = LExit;
    PatchBreaks();

    // Patch regular cases
    for (std::size_t i = 0; i < caseLabels.size(); ++i)
    {
        auto const caseBackpatchLine = caseBackpatchLines.at(i);

        auto const caseBackpatchNextImp =
            caseImplicitJumpToNextBackpatchLines.at(i);

        auto LCNext =
            (i + 1 != caseLabels.size()) ? caseLabels.at(i + 1) + 1 : LD + 1;
        auto LCNextImplicit =
            (i + 1 != caseLabels.size()) ? caseLabels.at(i + 1) + 2 : LD + 1;

        PatchJmpz(caseBackpatchLine, LCNext, rcmp);
        PatchJump(caseBackpatchNextImp, LCNextImplicit);
    }
}

void
CodeGenerator::Codegen(WhileStatement *node)
{
    // The algorithm of while statement generation

    //      L1:     Condition code
    //              If condition==0: jump to L2 (exit loop)
    //              Body code
    //              Jump to L1 (check condition again)
    //      L2:     ... Rest of code ...

    auto const condition_register = node->GetConditionExpr()->GetRegister();

    auto const L1 = GetCurrentAsmLineNumber();
    node->GetConditionExpr()->AcceptCodeGenerator(*this);
    auto const jzl2Filler =
        EmitFiller("While: If condition is zero, exit loop");
    node->GetBodyStatement()->AcceptCodeGenerator(*this);
    auto const jtl1Filler = EmitFiller("While: Check condition again");
    auto const L2         = GetCurrentAsmLineNumber();

    // Backpatch
    PatchJmpz(jzl2Filler, L2 + 1, condition_register);
    PatchJump(jtl1Filler, L1 + 1);

    PatchBreaks();
}

void
CodeGenerator::Codegen(CastExpression *node)
{
    node->GetExpr()->AcceptCodeGenerator(*this);
    auto const castType    = node->GetType(GetSymbolTable());
    auto const myRegister  = node->GetRegister();
    auto const exprRegister = node->GetExpr()->GetRegister();

    // We need to check that the expression we are casting is a different type
    // than the cast
    auto const innerExpressionType =
        node->GetExpr()->GetType(GetSymbolTable());

    // If both the cast type and the inner expression type are the same, we can
    // just skip the casting, but we should still emit a register from the cast
    // expression

    std::string opcode;
    if (castType == innerExpressionType)
    {
        opcode = (castType == NumberType::INT) ? "IASN" : "RASN";
    }
    else
    {
        opcode = (castType == NumberType::INT) ? "RTOI" : "ITOR";
    }

    EmitLine(opcode, myRegister, exprRegister);
}

void
CodeGenerator::Codegen(NotExpression *node)
{
    node->GetExpr()->AcceptCodeGenerator(*this);
    EmitLine("INQL", node->GetRegister(), node->GetExpr()->GetRegister(), 1);
}

void
CodeGenerator::Codegen(BinaryBoolExpression *node)
{
    auto const myRegister = node->GetRegister();
    auto const myOperator = node->GetOperator();

    if (myOperator == Operator::AND)
    {
        // The algorithm of AND expr generation with short-circuting

        // Let M be my register

        //          Check LHS
        //          Set M = LHS
        //          If M == 0: Jump to L1
        //          Check RHS
        //          Set M = RHS
        //  L1:     ... Rest of the code ...

        node->GetLhsExpr()->AcceptCodeGenerator(*this);
        EmitLine("IASN", myRegister, node->GetLhsExpr()->GetRegister());

        auto const jzl1Filler = EmitFiller("AND: if M == 0 jump to L1");

        node->GetRhsExpr()->AcceptCodeGenerator(*this);
        EmitLine("IASN", myRegister, node->GetRhsExpr()->GetRegister());

        auto const L1 = GetCurrentAsmLineNumber();

        // Backpatch
        PatchJmpz(jzl1Filler, L1 + 1, myRegister);
    }
    else if (myOperator == Operator::OR)
    {
        // The algorithm of OR expr generation with short-circuting

        // Let M be my register.

        //          Jump to L1
        //  L2:     Check RHS
        //          M = RHS
        //          Jump to L3
        //  L1:     Check LHS
        //          M = LHS
        //          If LHS == 0, Jump to L2 (Check RHS)
        //  L3:     ... Rest of the code ...

        auto const jtl1Filler = EmitFiller("OR: Jump to L1");

        auto const L2 = GetCurrentAsmLineNumber();
        node->GetRhsExpr()->AcceptCodeGenerator(*this);
        EmitLine("IASN", myRegister, node->GetRhsExpr()->GetRegister());
        auto const jtl3Filler = EmitFiller("OR: Jump to L3");

        auto const L1 = GetCurrentAsmLineNumber();
        node->GetLhsExpr()->AcceptCodeGenerator(*this);
        EmitLine("IASN", myRegister, node->GetLhsExpr()->GetRegister());

        auto const jzl2Filler = EmitFiller("OR: Jumpz to L2 if LHS=0");

        auto const L3 = GetCurrentAsmLineNumber();

        // Backpatch
        PatchJump(jtl1Filler, L1 + 1);
        PatchJump(jtl3Filler, L3 + 1);
        PatchJmpz(jzl2Filler, L2 + 1, myRegister);
    }
    else
    {
        node->GetLhsExpr()->AcceptCodeGenerator(*this);
        node->GetRhsExpr()->AcceptCodeGenerator(*this);

        auto       lhsRegister = node->GetLhsExpr()->GetRegister();
        auto       rhsRegister = node->GetRhsExpr()->GetRegister();
        auto const lhsType = node->GetLhsExpr()->GetType(GetSymbolTable());
        auto const rhsType = node->GetRhsExpr()->GetType(GetSymbolTable());

        // If both operands are ints, use the int variation by default.
        bool useInt        = true;
        bool shouldConvert = false;
        // If one of the operands is a float, convert the other one to int and
        // use
        // the float variation
        if (lhsType != rhsType)
        {
            shouldConvert = true;
        }

        if (shouldConvert && lhsType == NumberType::INT)
        {
            Register cvt;
            useInt = false;
            EmitLine("ITOR", cvt, lhsRegister);
            lhsRegister = cvt;
        }

        if (shouldConvert && rhsType == NumberType::INT)
        {
            Register cvt;
            useInt = false;
            EmitLine("ITOR", cvt, rhsRegister);
            rhsRegister = cvt;
        }

        if (lhsType == NumberType::FLOAT)
        {
            useInt = false;
        }

        std::string opcode;
        opcode.push_back(useInt ? 'I' : 'R'); // Choose the right prefix

        bool invertLogicalCondition = false; // If we need to use GTE, LTE ops

        // Now we know what variation we have to use, and we have both registers
        // in the proper types
        switch (node->GetOperator())
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
            invertLogicalCondition = true;
            break;
        case Operator::GTE:
            opcode.append("LSS");
            invertLogicalCondition = true;
            break;
        default:
            m_module.EmitFatalError(
                "invalid operator on boolean binary expression");
        }

        // Now let's generate the code
        EmitLine(opcode, myRegister, lhsRegister, rhsRegister);
        // If we need to invert the logical condition to get a "*-or-equals"
        // inst let's emit a NOT also
        if (invertLogicalCondition)
        {
            EmitLine("INQL", myRegister, myRegister, 1);
        }

        // That's it!
    }
}

void
CodeGenerator::Codegen(BinaryExpression *node)
{
    node->GetLhsExpr()->AcceptCodeGenerator(*this);
    node->GetRhsExpr()->AcceptCodeGenerator(*this);
    auto const myRegister  = node->GetRegister();
    auto       lhsRegister = node->GetLhsExpr()->GetRegister();
    auto       rhsRegister = node->GetRhsExpr()->GetRegister();

    auto const lhsType = node->GetLhsExpr()->GetType(GetSymbolTable());
    auto const rhsType = node->GetRhsExpr()->GetType(GetSymbolTable());

    // If both types are ints:
    //      - use the I-variation of the instruction.
    //      - this will be the default
    // Else (meaning one of the types are floats):
    //      - convert the other one to float
    //      - use the R-variation of the instruction
    bool useInt        = true;
    bool shouldConvert = false;
    // If one of the operands is a float, convert the other one to int and
    // use
    // the float variation
    if (lhsType != rhsType)
    {
        shouldConvert = true;
    }

    if (shouldConvert && lhsType == NumberType::INT)
    {
        Register cvt;
        useInt = false;
        EmitLine("ITOR", cvt, lhsRegister);
        lhsRegister = cvt;
    }

    if (shouldConvert && rhsType == NumberType::INT)
    {
        Register cvt;
        useInt = false;
        EmitLine("ITOR", cvt, rhsRegister);
        rhsRegister = cvt;
    }

    if (lhsType == NumberType::FLOAT)
    {
        useInt = false;
    }

    // The opcode naming is quite redundant, so let's not repeat ourselves
    std::string opcode;
    opcode.push_back(useInt ? 'I' : 'R'); // Choose the right prefix

    // Choose the instruction based on the operator

    switch (node->GetOperator())
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
        m_module.EmitFatalError("invalid operator in binary expression");
    }

    // Generate the code
    EmitLine(opcode, myRegister, lhsRegister, rhsRegister);
}

void
CodeGenerator::Codegen(IdExpression *node)
{
    // I want to SET the ID register to the symbol register, to sync them.
    auto const sym   = GetSymbolTable().Get(*node->GetID());
    node->m_register = sym->reg;

    // Now we can continue with no worries the ID will not refer to the symbol
}

void
CodeGenerator::Codegen(NumberExpression *node)
{
    auto const type = node->GetType(GetSymbolTable());
    if (type == NumberType::INT)
    {
        EmitLine("IASN", node->GetRegister(), node->GetNumber().i);
    }
    else
    {
        EmitLine("RASN", node->GetRegister(), node->GetNumber().d);
    }
}

void
CodeGenerator::Codegen(BreakStatement *node)
{
    UNUSED(node);
    auto const fillerLine = EmitFiller("BREAK: backpatch here");
    m_breakRequests.push_back(fillerLine);
}

unsigned
CodeGenerator::EmitFiller(std::string const &comment)
{
    m_module.m_assembly.push_back("FILLER: " + comment);
    return m_module.m_assembly.size() - 1;
}

void
CodeGenerator::PatchJmpz(unsigned line_no, unsigned jump_target, Register r)
{
    m_module.m_assembly.at(line_no) =
        "JMPZ " + std::to_string(jump_target) + " " + r.GetName();
}

void
CodeGenerator::PatchJump(unsigned line_no, unsigned jump_target)
{
    m_module.m_assembly.at(line_no) = "JUMP " + std::to_string(jump_target);
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode)
{
    m_module.m_assembly.push_back(opcode);
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r)
{
    m_module.m_assembly.push_back(opcode + " " + r.GetName());
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r1, Register r2)
{
    m_module.m_assembly.push_back(opcode + " " + r1.GetName() + " " + r2.GetName());
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r, int val)
{
    m_module.m_assembly.push_back(opcode + " " + r.GetName() + " " +
                                  std::to_string(val));
    return m_module.m_assembly.size() - 1;
}
unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r, int val1,
                        int val2)
{
    m_module.m_assembly.push_back(opcode + " " + r.GetName() + " " +
                                  std::to_string(val1) + " " +
                                  std::to_string(val2));
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r1, Register r2,
                        int val)
{
    m_module.m_assembly.push_back(opcode + " " + r1.GetName() + " " + r2.GetName() +
                                  " " + std::to_string(val));
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r1, Register r2,
                        Register r3)
{
    m_module.m_assembly.push_back(opcode + " " + r1.GetName() + " " + r2.GetName() +
                                  " " + r3.GetName());
    return m_module.m_assembly.size() - 1;
}

unsigned
CodeGenerator::EmitLine(std::string const &opcode, Register r, double val)
{
    std::array<char, 128> buf{};
    std::snprintf(buf.data(), buf.size(), "%.5g", val);
    m_module.m_assembly.push_back(opcode + " " + r.GetName() + " " +
                                  std::string(buf.data()));
    return m_module.m_assembly.size() - 1;
}

SymbolTable &
CodeGenerator::GetSymbolTable()
{
    return m_module.GetSymbolTable();
}

void
CodeGenerator::PatchBreaks()
{
    for (unsigned const br : m_breakRequests)
    {
        PatchJump(br, m_breakJumpDest + 1);
    }

    m_breakRequests.clear();
}

unsigned
CodeGenerator::GetCurrentAsmLineNumber() const
{
    return m_module.m_assembly.size();
}
