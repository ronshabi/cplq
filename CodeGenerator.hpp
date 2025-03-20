//===-- CodeGenerator.hpp -------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#pragma once

class Module;
class SymbolTable;
class Register;

#include "Ast.hpp"

class CodeGenerator
{
public:
    explicit CodeGenerator(Module &mod);

    void Codegen(Program *node);
    void Codegen(DeclarationBlock *node);
    void Codegen(StatementList *node);
    void Codegen(AssignStatement *node);
    void Codegen(InputStatement *node);
    void Codegen(OutputStatement *node);
    void Codegen(IfStatement *node);
    void Codegen(SwitchStatement *node);
    void Codegen(WhileStatement *node);
    void Codegen(CastExpression *node);
    void Codegen(NotExpression *node);
    void Codegen(BinaryBoolExpression *node);
    void Codegen(BinaryExpression *node);
    void Codegen(IdExpression *node);
    void Codegen(NumberExpression *node);
    void Codegen(BreakStatement *node);

private:
    unsigned EmitFiller(std::string const &comment);
    void PatchJmpz(unsigned line_no, unsigned jump_target, Register r);
    void PatchJump(unsigned line_no, unsigned jump_target);
    void PatchBreaks();
    unsigned EmitLine(std::string const &str);
    unsigned EmitLine(std::string const &opcode, Register r);
    unsigned EmitLine(std::string const &opcode, Register r1, Register r2);
    unsigned EmitLine(std::string const &opcode, Register r, int val);
    unsigned EmitLine(std::string const &opcode, Register r, int val1, int val2);
    unsigned EmitLine(std::string const &opcode, Register r1, Register r2,
                   int val);
    unsigned EmitLine(std::string const &opcode, Register r1, Register r2,
                   Register r3);
    unsigned EmitLine(std::string const &opcode, Register r, double val);

    SymbolTable &GetSymbolTable();


    unsigned GetCurrentAsmLineNumber() const;

    std::vector<unsigned> m_breakRequests{};
    unsigned              m_breakJumpDest = 0;

    Module &m_module;
};