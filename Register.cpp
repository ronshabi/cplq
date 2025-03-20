#include "Register.hpp"

unsigned Register::s_nextID = 0;

std::string
Register::GetName() const
{
    std::string out = "t";
    out.append(std::to_string(m_id));
    return out;
}
