//===-- Types.hpp ---------------------------------------------------------===//
//
// This file provides common types and definitions for objects in the CPL
// language. This file is also used by Flex and Bison.
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#pragma once

/// CPL's operators
enum class Operator
{
    EQ,    // ==
    NEQ,   // !=
    LT,    // <
    GT,    // >
    LTE,   // <=
    GTE,   // >=
    PLUS,  // +
    MINUS, // -
    MUL,   // *
    DIV,   // /
    AND,   // &&
    OR     // ||
};

/// CPL's data types
enum class NumberType
{
    INT,  // int
    FLOAT // float
};

/// Get a stringified representation of a NumberType ("Int" / "Float")
const char *NumberTypeToString(NumberType numberType);

/// Get a stringified representation of an operator (+-/...)
const char *OperatorToString(Operator op);

/// CPL's number object type
struct Number
{
    NumberType type; ///< Data type

    bool hasValue; ///< Was this number assigned a compile-time value?
    union {
        int    i; ///< Int value
        double d; ///< Double value
    };
};