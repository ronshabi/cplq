#pragma once

#include <cassert>
#include <string>
#include <vector>

#define UNUSED(x) ((void)(x))

template <typename T, typename ToStringFn>
std::string
VectorToStringImpl(std::vector<T> const &vec, ToStringFn fn)
{
    std::string o = "[";

    for (std::size_t i = 0; i < vec.size(); ++i)
    {

        o.append(fn(vec[i]));

        if (i + 1 != vec.size())
        {
            o.append(", ");
        }
    }

    o.push_back(']');

    return o;
}

std::string VectorToString(std::vector<std::string *> const &vec);