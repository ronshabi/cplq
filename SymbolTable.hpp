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
        , lineNumber(line_no)
    {
    }

    Symbol(NumberType type, unsigned line_no)
        : lineNumber(line_no)
    {
        value           = Number();
        value.hasValue = false;
        value.type      = type;
    }

    std::string        ToString() const;

    Register reg;
    Number   value;
    unsigned lineNumber;
};

class SymbolTable
{
public:
    SymbolTable() = default;

    // void set_register(std::string const &symbol_name, Register reg);
    std::optional<Symbol> Get(std::string const &symbol_name) const;
    void                  Add(std::string const &symbol_name, Symbol sym);

    void Dump() const;

private:
    std::map<std::string, Symbol> m_table;
};