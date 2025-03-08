#include "Utils.hpp"

std::string vector_to_string(std::vector<std::string *> const &vec)
{
    return vector_to_string_impl(vec, [=](std::string *s) { return *s; });
}
