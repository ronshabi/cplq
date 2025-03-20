//===-- Module.cpp --------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#include "Module.hpp"
#include "Print.hpp"

#include <cstdarg>
#include <fstream>

Module::Module(std::string const &file_name)
    : m_fileName(file_name)
    , m_semanticAnalyzer(*this)
    , m_codeGenerator(*this)
{
    CheckCorrectFileExt();

    if (m_status == ModuleStatus::InvalidExtension)
    {
        EmitFatalError("unsupported file extension");
        return;
    }

    TryToOpenFile();

    if (m_status == ModuleStatus::FileNotFound)
    {
        EmitFatalError("file not found");
        return;
    }
}

Module::~Module()
{
    if (m_filePtr)
    {
        fclose(m_filePtr);
        m_filePtr = nullptr;
    }
}

void
Module::EmitError(const char *fmt, ...)
{
    FPrintfColor(stderr, kAnsiBold, "%s:%d: ", m_fileName.c_str(),
                  m_currentLineNo);
    FPrintfColor(stderr, kAnsiBoldRed, "error: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    printf("\n");

    // Semantic Error will be the default error type, if we want to change it
    // We can just call SetStatus after EmitError
    SetStatus(ModuleStatus::SemanticError);
}

void
Module::EmitFatalError(const char *fmt, ...)
{
    FPrintfColor(stderr, kAnsiBold, "%s: ", m_fileName.c_str());
    FPrintfColor(stderr, kAnsiBoldRed, "fatal: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    printf("\n");
}

void
Module::SemanticAnalyze()
{
    if (!IsOK())
    {
        return; // No need to Analyze syntactically malformed code.
    }

    m_semanticAnalyzer.Analyze(reinterpret_cast<Program *>(m_ast));
}

void
Module::CodeGen()
{
    if (!IsOK())
    {
        return; // No need to generate assembly for invalid source code
    }

    m_codeGenerator.Codegen(reinterpret_cast<Program *>(m_ast));

    // Create the file
    const std::string output_file_name =
        m_fileName.substr(0, m_fileName.size() - 2) + "qud";
    std::ofstream output_file(output_file_name);
    if (output_file.bad())
    {
        EmitFatalError("Can't create an output file '%s' due to OS error",
                         output_file_name.c_str());
        m_status = ModuleStatus::OSError;
        return;
    }

    for (auto const &line : m_assembly)
    {
        output_file << line << "\n";
    }

    output_file.close();
}

void
Module::CheckCorrectFileExt()
{
    auto const dot = m_fileName.find_last_of('.');
    if (dot == std::string::npos)
    {
        m_status = ModuleStatus::InvalidExtension;
        return;
    }

    auto const extension = m_fileName.substr(dot);
    if (extension != ".ou")
    {
        m_status = ModuleStatus::InvalidExtension;
    }
}

void
Module::TryToOpenFile()
{
    m_filePtr = fopen(m_fileName.c_str(), "r");
    if (m_filePtr == nullptr)
    {
        m_status = ModuleStatus::FileNotFound;
        return;
    }

    yyin = m_filePtr;
}
