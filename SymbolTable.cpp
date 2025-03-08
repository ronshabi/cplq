#include "SymbolTable.hpp"
#include <iostream>
#include <iomanip>

std::optional<Symbol> SymbolTable::get(std::string const &symbol_name)
{
    auto const it = m_table.find(symbol_name);
    if (it == m_table.end())
    {
        return {};
    }

    return {it->second};
}

void SymbolTable::add(std::string const &symbol_name, Symbol sym)
{
    m_table.insert({symbol_name, sym});
}

void SymbolTable::dump() const
{
    std::cout << "\033[1;33mSymbol Table:\033[0m\n";
    for (auto const& [symbol_name, symbol_value] : m_table)
    {
        std::cout << "'" << symbol_name << "' -> " << symbol_value.to_string() << "\n";     
    }
}

std::string Symbol::to_string() const {
    std::string o = "Symbol<" + std::to_string(line_number);

    if (value.has_value) {
        if (value.type == NumberType::INT) {
            o.append(", Int>(");
            o.append(std::to_string(value.i));

        } else {
            o.append(", Float>(");
            o.append(std::to_string(value.d));
        }
        o.append(")");
    } else {
        o.append(", NoValue>");
    }

    return o;
}
