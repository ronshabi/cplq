#pragma once
#include "Register.hpp"
#include "Types.hpp"
#include <map>
#include <optional>
#include <string>

struct Symbol
{
    Symbol(Number val, unsigned line_no)
        : reg()
        , value(val)
        , line_number(line_no)
    {
    }

    Symbol(NumberType type, unsigned line_no)
        : line_number(line_no)
    {
        value           = Number();
        value.has_value = false;
        value.type      = type;
    }

    static std::string get_asm_name(std::string const &symbol_name);
    std::string        to_string() const;

    Register reg;
    Number   value;
    unsigned line_number;
};

class SymbolTable
{
public:
    SymbolTable() = default;

    // void set_register(std::string const &symbol_name, Register reg);
    std::optional<Symbol> get(std::string const &symbol_name) const;
    void                  add(std::string const &symbol_name, Symbol sym);

    void dump() const;

private:
    std::map<std::string, Symbol> m_table;
};