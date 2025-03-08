#include "SymbolTable.hpp"
std::optional<Number> SymbolTable::get(std::string const& symbol_name) {
    auto const it = m_table.find(symbol_name);
    if (it == m_table.end()) {
        return {};
    }

    return {it->second};

}

void SymbolTable::add(std::string const& symbol_name, Number value) {
    m_table[symbol_name] = value;
}

void SymbolTable::add(std::string const& symbol_name, NumberType type) {
    auto num = Number();
    num.type = type;
    num.has_value = false;
    m_table[symbol_name] = num; 
}

void SymbolTable::dump() const {
    printf("SYMBOL TABLE");
    for (auto const &item : m_table) {
        printf("%-20s\n", item.first.c_str());
    }
}
