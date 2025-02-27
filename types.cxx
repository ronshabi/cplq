#include "types.hxx"

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

} // namespace cpl