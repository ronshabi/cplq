#pragma once

#include <string>

#include "Ast.hpp"
#include "ModuleStatus.hpp"

extern "C" int yyparse();
extern FILE *yyin;

class Module
{
public:
    Module(std::string const &file_name);
    ~Module();

    Module(Module const&) = delete;
    Module& operator=(Module const&) = delete;

    void emit_error(const char *fmt, ...) __attribute__((format(printf, 2, 3)));

    void emit_fatal_error(const char *fmt, ...)
        __attribute__((format(printf, 2, 3)));

    void set_current_position(unsigned line_no, unsigned column_no)
    {
        m_current_line_no   = line_no;
        m_current_column_no = column_no;
    }

    int get_error_code() const {
        return is_ok() ? 0 : 1;
    }

    int is_ok() const {
        return m_status == ModuleStatus::Ok;
    }

    void parse() {
        if (is_ok()) {
            yyparse();
        }
    }

    void set_ast_root(AstNode *root) {
        m_ast = root;
    }

private:
    void check_correct_file_extension();
    void try_to_open_file();

    ModuleStatus m_status = ModuleStatus::Ok;
    std::string  m_file_name;
    unsigned     m_current_line_no   = 0;
    unsigned     m_current_column_no = 0;
    FILE        *m_file_ptr          = nullptr;
    AstNode     *m_ast               = nullptr;
};