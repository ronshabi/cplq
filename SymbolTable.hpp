#pragma once
#include "Types.hpp"
#include <map>
#include <optional>
#include <string>

struct Symbol
{
    Symbol(Number val, unsigned line_no)
        : value(val)
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

    std::string to_string() const;

    Number   value;
    unsigned line_number;
};

class SymbolTable
{
public:
    SymbolTable() = default;

    std::optional<Symbol> get(std::string const &symbol_name) const;
    void                  add(std::string const &symbol_name, Symbol sym);

    void dump() const;

private:
    std::map<std::string, Symbol> m_table;
};