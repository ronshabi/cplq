#pragma once

#include <string>

struct Register
{
public:
    Register(): m_id(++s_next_id) {}

    unsigned id() const {
        return m_id;
    }

    std::string name() const;

private:
    static unsigned s_next_id;
    unsigned m_id;
};
