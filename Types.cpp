//===-- Types.cpp ---------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#include "Types.hpp"

const char *
NumberTypeToString(NumberType numberType)
{
    if (numberType == NumberType::FLOAT)
    {
        return "Float";
    }

    return "Int";
}

const char *
OperatorToString(Operator op)
{
    switch (op)
    {
    case Operator::AND:
        return "&&";
    case Operator::DIV:
        return "/";
    case Operator::EQ:
        return "==";
    case Operator::GT:
        return "GT";
    case Operator::GTE:
        return "GTE";
    case Operator::LT:
        return "LT";
    case Operator::LTE:
        return "LTE";
    case Operator::MINUS:
        return "-";
    case Operator::MUL:
        return "*";
    case Operator::NEQ:
        return "!=";
    case Operator::OR:
        return "||";
    case Operator::PLUS:
        return "+";
    default:
        return "SHOULD_IMPLEMENT";
    }
}