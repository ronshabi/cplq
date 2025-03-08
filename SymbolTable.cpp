#include "SymbolTable.hpp"
std::optional<Symbol> SymbolTable::get(std::string const& symbol_name) {
    auto const it = m_table.find(symbol_name);
    if (it == m_table.end()) {
        return {};
    }

    return {it->second};

}

void SymbolTable::add(std::string const& symbol_name, Symbol sym) {
    m_table.insert({symbol_name, sym});
}

void SymbolTable::dump() const {
    printf("SYMBOL TABLE\n");
    for (auto const &item : m_table) {
        printf("%-20s\n", item.first.c_str());
    }
}
