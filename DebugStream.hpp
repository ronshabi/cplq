//===-- DebugStream.hpp ---------------------------------------------------===//
//
//
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
