#include "Types.hpp"

namespace cpl
{
const char *to_string(NumberType number_type)
{
    if (number_type == NumberType::FLOAT)
    {
        return "Float";
    }

    return "Int";
}

const char *to_string(Operator op)
{
    switch (op)
    {
    case Operator::PLUS:
        return "+";
    default:
        return "SHOULD_IMPLEMENT";
    }
}

} // namespace cpl