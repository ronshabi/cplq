//===-- Register.hpp ------------------------------------------------------===//
//
// This file defines the Register structure, which represents a register with a
// unique identifier
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//


#pragma once

#include <string>

/// \class Register
/// \brief Represents a register with a unique identifier.
struct Register
{
public:
    Register();
    Register(Register const &other);

    /// \brief Copy assignment operator.
    ///
    /// Assigns the ID of the given Register to this Register.
    ///
    /// \param other The Register to copy.
    /// \return A reference to this Register.
    Register &
    operator=(Register const &other);

    /// \brief Returns the name of the Register.
    ///
    /// The name is constructed by appending the Register's ID to the string
    /// "t".
    ///
    /// \return The name of the Register as a string.
    std::string GetName() const;

private:
    static unsigned s_nextID; ///< Static member to track the next available ID.
    unsigned        m_id;     ///< Unique identifier for the Register.
};
