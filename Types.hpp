#pragma once
#include <optional>
enum class Operator
{
    EQ,
    NEQ,
    LT,
    GT,
    LTE,
    GTE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    AND,
    OR
};

enum class NumberType
{
    INT,
    FLOAT
};

const char *number_type_to_string(NumberType number_type);
const char *operator_to_string(Operator op);

struct Number
{
    NumberType type;

    bool has_value;
    union {
        int    i;
        double d;
    };

    std::optional<double>
    get_value_float() const
    {
        if (has_value)
        {
            if (type == NumberType::INT)
            {
                return static_cast<double>(i);
            }
            else
            {
                return d;
            }
        }

        return std::nullopt;
    }
};