#pragma once

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
};