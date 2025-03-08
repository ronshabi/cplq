#include "SymbolTable.hpp"
#include <iostream>

std::optional<Symbol> SymbolTable::get(std::string const &symbol_name) const
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

std::string Symbol::get_asm_name(std::string const& symbol_name) {
    std::string o = "v_";

    for (char const ch : symbol_name) {
        // Encode uppercase characters in original variable name for the quad name.
        if (ch >= 'A' && ch <= 'Z') {
            o.push_back(ch + ('a' - 'A'));
            o.push_back('_');
        } else {
            o.push_back(ch);
        }
    }

    return o;
}

std::string Symbol::to_string() const {
    std::string o = "Symbol<@" + std::to_string(line_number);

    if (value.type == NumberType::INT) {
        o.append(", Int>(");
        if (value.has_value) {
            o.append(std::to_string(value.i));
        } else {
            o.append("None");
        }
    } else {
        o.append(", Float>(");
        if (value.has_value) {
            o.append(std::to_string(value.d));
        } else {
            o.append("None");
        }
    }
    
    o.append(")");

    return o;
}
