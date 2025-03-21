//===-- Utils.hpp ---------------------------------------------------------===//
//
// Common utility functions
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <string>
#include <vector>

/// Used to silence "unused parameter ..." warnings.
#define UNUSED(x) ((void)(x))

/// The actual function making a [..., ..., ...] list from a vector, using
/// a user-provided function `fn`. This isn't used directly by main code, but by
/// much more convenient wrapper functions. 
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

/// Wrapper function for converting a vector of string pointers into
/// a string in the form of [a, b, ...] (simillar to Python's list print).
std::string VectorToString(std::vector<std::string *> const &vec);