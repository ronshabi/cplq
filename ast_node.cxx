#pragma once

#include <vector>
#include <string>

#include "cpl_types.hxx"

class ASTNode {
public:
    virtual ~ASTNode();
};

class Decl : public ASTNode
{
private:
    std::vector<std::string> m_ids;
    cpl::NumberType m_type;
};

class Expression : public ASTNode
{
    
}