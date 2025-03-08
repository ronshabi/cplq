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
