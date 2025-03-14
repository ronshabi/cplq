#include "Register.hpp"

unsigned Register::s_next_id = 0;

std::string Register::name() const {
    std::string out = "t";
    out.append(std::to_string(m_id));
    return out;
}
