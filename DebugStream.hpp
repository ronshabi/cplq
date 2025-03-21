//===-- DebugStream.hpp ---------------------------------------------------===//
//
// Provides a quick and pretty way to print debug info to stdout. Not used
// In the final product, but very useful in debugging.
//
// To enable this, compile with -DDEBUG.
//
//===----------------------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>

#ifdef DEBUG
#define DBG(x)                                                              \
    std::cout << "\033[90m" << __FILE__ "::" << __PRETTY_FUNCTION__ << "] " \
              << x << "\033[0m\n";
#else
#define DBG(x)
#endif
