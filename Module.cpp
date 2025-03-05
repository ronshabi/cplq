#include "Module.hpp"
#include "Print.hpp"

#include <cstdarg>

Module::Module(std::string const& file_name)
    : m_file_name(file_name)
{
    check_correct_file_extension();

    if (m_status == ModuleStatus::InvalidExtension) {
        emit_fatal_error("unsupported file extension");
        return;
    }

    try_to_open_file();

    if (m_status == ModuleStatus::FileNotFound) {
        emit_fatal_error("file not found");
        return;
    }
}

Module::~Module()
{
    if (m_file_ptr) {
        fclose(m_file_ptr);
        m_file_ptr = nullptr;
    }
}

void Module::emit_error(const char *fmt, ...) {
    fprintf_color(stdout, kAnsiBold, "%s:%d: ", m_file_name.c_str(), m_current_line_no);
    fprintf_color(stdout, kAnsiBoldRed, "error: ");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void Module::emit_fatal_error(const char *fmt, ...) {
    fprintf_color(stdout, kAnsiBold, "%s: ", m_file_name.c_str());
    fprintf_color(stdout, kAnsiBoldRed, "fatal: ");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void Module::check_correct_file_extension() {
    auto const dot = m_file_name.find_last_of('.');
    if (dot == std::string::npos) {
        m_status = ModuleStatus::InvalidExtension;
        return;
    }

    auto const extension = m_file_name.substr(dot);
    if (extension != ".ou") {
        m_status = ModuleStatus::InvalidExtension;
    }
}

void Module::try_to_open_file() {
    m_file_ptr = fopen(m_file_name.c_str(), "r");
    if (m_file_ptr == nullptr) {
        m_status = ModuleStatus::FileNotFound;
        return;
    }

    yyin = m_file_ptr;
}
