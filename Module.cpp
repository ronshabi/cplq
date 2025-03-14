#include "Module.hpp"
#include "Print.hpp"

#include <cstdarg>
#include <fstream>

Module::Module(std::string const &file_name)
    : m_file_name(file_name)
    , m_semantic_analyzer(*this)
    , m_code_generator(*this)
{
    check_correct_file_extension();

    if (m_status == ModuleStatus::InvalidExtension)
    {
        emit_fatal_error("unsupported file extension");
        return;
    }

    try_to_open_file();

    if (m_status == ModuleStatus::FileNotFound)
    {
        emit_fatal_error("file not found");
        return;
    }
}

Module::~Module()
{
    if (m_file_ptr)
    {
        fclose(m_file_ptr);
        m_file_ptr = nullptr;
    }
}

void
Module::emit_error(const char *fmt, ...)
{
    fprintf_color(stderr, kAnsiBold, "%s:%d: ", m_file_name.c_str(),
                  m_current_line_no);
    fprintf_color(stderr, kAnsiBoldRed, "error: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    printf("\n");

    // Semantic Error will be the default error type, if we want to change it
    // We can just call set_status after emit_error
    set_status(ModuleStatus::SemanticError);
}

void
Module::emit_fatal_error(const char *fmt, ...)
{
    fprintf_color(stdout, kAnsiBold, "%s: ", m_file_name.c_str());
    fprintf_color(stdout, kAnsiBoldRed, "fatal: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    printf("\n");
}

void
Module::semantic_analyze()
{
    if (!is_ok())
    {
        return; // No need to analyze syntactically malformed code.
    }

    DBG("Calling Semantic Analyzer");
    m_semantic_analyzer.analyze(reinterpret_cast<Program *>(m_ast));
}

void Module::codegen() {
    if (!is_ok()) {
        return; // No need to generate assembly for invalid source code
    }

    DBG("Calling CodeGen");
    m_code_generator.codegen(reinterpret_cast<Program *>(m_ast));

    // Create the file
    const std::string output_file_name = m_file_name.substr(0, m_file_name.size() - 2) + "qud";
    std::ofstream output_file(output_file_name);
    if (output_file.bad()) {
        emit_fatal_error("Can't create an output file '%s' due to OS error", output_file_name.c_str());
        m_status = ModuleStatus::OSError;
        return;
    }

    for (const auto& line : m_assembly) {
        output_file << line << "\n";
    }

    output_file.close();
    DBG("Created output file");
}

std::string &
Module::add_assembly_line(std::string const &line)
{
    m_assembly.push_back(line);
    return m_assembly.back();
}

void
Module::check_correct_file_extension()
{
    auto const dot = m_file_name.find_last_of('.');
    if (dot == std::string::npos)
    {
        m_status = ModuleStatus::InvalidExtension;
        return;
    }

    auto const extension = m_file_name.substr(dot);
    if (extension != ".ou")
    {
        m_status = ModuleStatus::InvalidExtension;
    }
}

void
Module::try_to_open_file()
{
    m_file_ptr = fopen(m_file_name.c_str(), "r");
    if (m_file_ptr == nullptr)
    {
        m_status = ModuleStatus::FileNotFound;
        return;
    }

    yyin = m_file_ptr;
}
