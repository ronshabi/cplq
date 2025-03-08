#pragma once

class Module;

#include "Ast.hpp"

class CodeGenerator
{
public:
    CodeGenerator(Module &mod);

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
private:
    Module& m_module;
};