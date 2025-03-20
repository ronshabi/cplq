//===-- ModuleStatus.hpp --------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#pragma once

/// \brief Status of a module.
enum class ModuleStatus
{
    Ok,                ///< Module is in a valid state.
    FileNotFound,      ///< Module file was not found.
    InvalidExtension,  ///< Module file has an invalid extension.
    SyntaxError,       ///< Module contains syntax errors.
    SemanticError,     ///< Module contains semantic errors.
    OSError,           ///< An operating system error occurred.
};