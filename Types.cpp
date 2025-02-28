#include "Types.hpp"

const char *number_type_to_string(NumberType number_type)
{
    if (number_type == NumberType::FLOAT)
    {
        return "Float";
    }

    return "Int";
}

const char *operator_to_string(Operator op)
{
    switch (op)
    {
    case Operator::PLUS:
        return "+";
    default:
        return "SHOULD_IMPLEMENT";
    }
}