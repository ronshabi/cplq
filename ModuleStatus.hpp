#pragma once

enum class ModuleStatus
{
    Ok,
    FileNotFound,
    InvalidExtension,
    SyntaxError,
    SemanticError
};