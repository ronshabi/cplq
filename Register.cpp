//===-- Register.cpp ------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//


#include "Register.hpp"

unsigned Register::s_nextID = 0;

Register::Register()
    : m_id(++s_nextID)
{
}

/// \brief Copy constructor.
///
/// Creates a new Register with the same ID as the given Register.
///
/// \param other The Register to copy.
Register::Register(Register const &other)
    : m_id(other.m_id)
{
}

Register &
Register::operator=(Register const &other)
{
    this->m_id = other.m_id;
    return *this;
}

std::string
Register::GetName() const
{
    std::string out = "t";
    out.append(std::to_string(m_id));
    return out;
}
