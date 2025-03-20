//===----------------------------------------------------------------------===//
// CPL to Quad Transpiler
// Author: Ron Shabi
//===----------------------------------------------------------------------===//

#pragma once

#include <string>

#include "Ast.hpp"
#include "CodeGenerator.hpp"
#include "DebugStream.hpp"
#include "ModuleStatus.hpp"
#include "SemanticAnalyzer.hpp"
#include "SymbolTable.hpp"

extern "C" int yyparse();
extern FILE   *yyin;

/// \class Module
/// \brief Represents a compilation module.
/// This class encapsulates the parsing, semantic analysis, and code generation
/// phases of a compilation unit. It manages the module's status, input file,
/// AST, symbol table, and generated assembly.
class Module
{
public:
    Module(std::string const &file_name);
    ~Module();

    Module(Module const &)            = delete;
    Module &operator=(Module const &) = delete;

    /// \brief Emits an error message to stderr.
    ///
    /// This function prints an error message with the specified format to the
    /// stderr. The message includes the module's file name and
    /// line number.
    //
    // The module's status is set to `SemanticError` by default.
    ///
    /// \param fmt The format string, compatible with printf.
    /// \param ... Additional arguments for the format string.
    void EmitError(const char *fmt, ...) __attribute__((format(printf, 2, 3)));

    /// \brief Emits a fatal error message to stderr.
    ///
    /// This function prints a fatal error message with the specified format
    /// to stderr. The message includes the module's file
    /// name, but not the line number, because a fatal error is relevant to the
    /// whole file, and not to a specific line.
    ///
    /// \param fmt The format string, compatible with printf.
    /// \param ... Additional arguments for the format string.
    void EmitFatalError(const char *fmt, ...)
        __attribute__((format(printf, 2, 3)));

    void
    SetCurrentPosition(unsigned line_no)
    {
        m_currentLineNo = line_no;
    }

    /// \brief Gets the error code.
    /// \return 0 if the module is ok, 1 otherwise.
    int
    GetErrorCode() const
    {
        return IsOK() ? 0 : 1;
    }

    /// \brief Returns true if the module has no errors whatsoever, false
    /// otherwise.
    
    int
    IsOK() const
    {
        return m_status == ModuleStatus::Ok;
    }

    /// \brief Parses the input file, calling Bison.
    void
    Parse()
    {
        if (IsOK())
        {
            yyparse();
        }
    }

    /// \brief Sets the AST root node.
    /// \param root The AST root node.
    void
    SetAstRoot(AstNode *root)
    {
        m_ast = root;
    }

    /// \brief Sets the module status.
    /// \param status The module status.
    void
    SetStatus(ModuleStatus status)
    {
        if (status == ModuleStatus::Ok)
        {
            printf("!!!!!!!! You can't OK a bad module\n");
        }

        m_status = status;
    }

    /// \brief Dumps the AST.
    ///\note Used for debugging.
    void
    DumpAst() const
    {
        DBG("Dump AST");
        m_ast->Dump();
    }

    /// \brief Dumps the symbol table.
    ///\note Used for debugging.
    void
    DumpSymbolTable()
    {
        m_semanticAnalyzer.GetSymbolTable().Dump();
    }

    /// \brief Performs semantic analysis (calling SemanticAnalyzer) on the
    /// module.
    ///
    /// This function initiates semantic analysis on the module's AST if the
    /// module is in a valid state. If the module is not valid, the function
    /// returns early without performing the analysis.
    void SemanticAnalyze();

    /// \brief Generates code for the module.
    ///
    /// This function generates assembly code from the module's AST if the
    /// module is in a valid state. The output is written to a file with a
    /// `.qud` extension.
    ///
    /// If the module is not valid or an OS error occurs while creating the
    /// output file, the function returns early.
    void CodeGen();

    SymbolTable &
    GetSymbolTable()
    {
        return m_symbolTable;
    }

    friend CodeGenerator;

private:
    void CheckCorrectFileExt();
    void TryToOpenFile();

    ModuleStatus     m_status = ModuleStatus::Ok;
    std::string      m_fileName;
    unsigned         m_currentLineNo = 0;
    FILE            *m_filePtr        = nullptr;
    AstNode         *m_ast             = nullptr;
    SemanticAnalyzer m_semanticAnalyzer;
    SymbolTable      m_symbolTable;
    CodeGenerator    m_codeGenerator;

    /// \brief The generated assembly.
    std::vector<std::string> m_assembly{};
};