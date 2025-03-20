#pragma once

#include <cstdio>

/// \brief ANSI escape code for bold text.
constexpr const char *kAnsiBold = "\033[1m";
/// \brief ANSI escape code for bold red text.
constexpr const char *kAnsiBoldRed = "\033[1;31m";
/// \brief ANSI escape code for resetting the font/color.
constexpr const char *kAnsiReset = "\033[0m";

/// \brief Prints a formatted string to a file with color.
///
/// \param f The file pointer to write to.
/// \param color The ANSI escape code for the desired color.
/// \param fmt The format string, compatible with printf.
/// \param ... Additional arguments for the format string.
///
/// \note Uses `__attribute__((format(printf, 3, 4)))` for format checking.
__attribute__((format(printf, 3, 4))) void FPrintfColor(FILE       *f,
                                                         const char *color,
                                                         const char *fmt, ...);