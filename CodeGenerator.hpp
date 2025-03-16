#pragma once

class Module;
class SymbolTable;
class Register;

#include "Ast.hpp"

class CodeGenerator
{
public:
    explicit CodeGenerator(Module &mod);

    void codegen(Program *node);
    void codegen(DeclarationBlock *node);
    void codegen(StatementList *node);
    void codegen(AssignStatement *node);
    void codegen(InputStatement *node);
    void codegen(OutputStatement *node);
    void codegen(IfStatement *node);
    void codegen(SwitchStatement *node);
    void codegen(WhileStatement *node);
    void codegen(CastExpression *node);
    void codegen(NotExpression *node);
    void codegen(BinaryBoolExpression *node);
    void codegen(BinaryExpression *node);
    void codegen(IdExpression *node);
    void codegen(NumberExpression *node);
    void codegen(BreakStatement *node);

private:
    void emit_line(std::string const &str);
    void emit_line(std::string const &opcode, Register r);
    void emit_line(std::string const &opcode, Register r1, Register r2);
    void emit_line(std::string const &opcode, Register r, int val);
    void emit_line(std::string const &opcode, Register r, int val1, int val2);
    void emit_line(std::string const &opcode, Register r1, Register r2,
                   int val);
    void emit_line(std::string const &opcode, Register r1, Register r2,
                   Register r3);
    void emit_line(std::string const &opcode, Register r, double val);

    SymbolTable &symbol_table();

    unsigned current_asm_line_number() const;

    Module &m_module;
};