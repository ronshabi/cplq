//===-- SymbolTable.hpp ---------------------------------------------------===//
//
// This file defines a Symbol structure and a SymbolTable class for
// managing program symbols. The Symbol structure includes a value, register,
// and line number, while the SymbolTable class allows adding, retrieving, and
// dumping symbols using a map for efficient lookup
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

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
        value          = Number();
        value.hasValue = false;
        value.type     = type;
    }

    std::string ToString() const;

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