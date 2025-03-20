#include "Utils.hpp"

std::string
VectorToString(std::vector<std::string *> const &vec)
{
    return VectorToStringImpl(vec, [=](std::string *s) { return *s; });
}
