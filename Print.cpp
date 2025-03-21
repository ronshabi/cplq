//===-- Print.cpp ---------------------------------------------------------===//
//
// Author: Ron Shabi
//
//===----------------------------------------------------------------------===//

#include "Print.hpp"
#include <cstdarg>

__attribute__((format(printf, 3, 4))) void
FPrintfColor(FILE *f, const char *color, const char *fmt, ...)
{
    // Print the color
    fprintf(f, "%s", color);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    // Reset the terminal color
    fprintf(f, "%s", kAnsiReset);
}
