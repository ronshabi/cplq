#pragma once

namespace cpl
{
enum class OperatorType
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
    DIV
};

enum class NumberType
{
    INT,
    FLOAT
};

struct Number
{
    NumberType type;

    union
    {
        int    i;
        double d;
    };
};
}    // namespace cpl
