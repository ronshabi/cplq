//===-- SymbolTable.cpp ---------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//


#include "SymbolTable.hpp"
#include <iostream>

std::optional<Symbol>
SymbolTable::Get(std::string const &symbol_name) const
{
    auto const it = m_table.find(symbol_name);
    if (it == m_table.end())
    {
        return {};
    }

    return {it->second};
}

void
SymbolTable::Add(std::string const &symbol_name, Symbol sym)
{
    m_table.insert({symbol_name, sym});
}

void
SymbolTable::Dump() const
{
    std::cout << "\033[1;33mSymbol Table:\033[0m\n";
    if (m_table.empty())
    {
        std::cout << "Empty\n";
        return;
    }
    for (auto const &[symbolName, symbolValue] : m_table)
    {
        std::cout << "'" << symbolName << "' -> " << symbolValue.ToString()
                  << "\n";
    }
}

std::string
Symbol::ToString() const
{
    std::string o =
        "Symbol<Reg: " + reg.GetName() + ", Line: " + std::to_string(lineNumber);

    if (value.type == NumberType::INT)
    {
        o.append(", Int>(");
        if (value.hasValue)
        {
            o.append(std::to_string(value.i));
        }
        else
        {
            o.append("None");
        }
    }
    else
    {
        o.append(", Float>(");
        if (value.hasValue)
        {
            o.append(std::to_string(value.d));
        }
        else
        {
            o.append("None");
        }
    }

    o.append(")");

    return o;
}
