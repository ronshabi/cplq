#pragma once

#include <string>

struct Register
{
public:
    Register()
        : m_id(++s_next_id)
    {
    }
    Register(Register const &other)
        : m_id(other.m_id)
    {
    }
    Register &
    operator=(Register const &other)
    {
        this->m_id = other.m_id;
        return *this;
    }

    unsigned
    id() const
    {
        return m_id;
    }

    std::string name() const;

private:
    static unsigned s_next_id;
    unsigned        m_id;
};
