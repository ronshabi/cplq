#pragma once

#include <string>

/// \class Register
/// \brief Represents a register with a unique identifier.
struct Register
{
public:
    Register()
        : m_id(++s_nextID)
    {
    }

    /// \brief Copy constructor.
    ///
    /// Creates a new Register with the same ID as the given Register.
    ///
    /// \param other The Register to copy.
    Register(Register const &other)
        : m_id(other.m_id)
    {
    }

    /// \brief Copy assignment operator.
    ///
    /// Assigns the ID of the given Register to this Register.
    ///
    /// \param other The Register to copy.
    /// \return A reference to this Register.

    Register &
    operator=(Register const &other)
    {
        this->m_id = other.m_id;
        return *this;
    }

    /// \brief Returns the name of the Register.
    ///
    /// The name is constructed by appending the Register's ID to the string
    /// "t".
    ///
    /// \return The name of the Register as a string.
    std::string GetName() const;

private:
    static unsigned
             s_nextID; ///< Static member to track the next available ID.
    unsigned m_id;      ///< Unique identifier for the Register.
};
