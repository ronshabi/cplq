//===-- Types.hpp ---------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

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

const char *NumberTypeToString(NumberType numberType);
const char *OperatorToString(Operator op);

struct Number
{
    NumberType type;

    bool hasValue;
    union {
        int    i;
        double d;
    };

};