#pragma once
#include "Types.hpp"
#include <string_view>
#include <string>
#include <map>
#include <optional>

class SymbolTable
{
public:
    SymbolTable() = default;
  
    std::optional<Number> get(std::string const& symbol_name);
    void add(std::string const& symbol_name, Number value);
    void add(std::string const& symbol_name, NumberType type);

    void dump() const;

private:
    std::map<std::string, Number> m_table;
};