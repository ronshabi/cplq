#pragma once

#include <cstdio>

constexpr const char *kAnsiBold    = "\033[1m";
constexpr const char *kAnsiBoldRed = "\033[1;31m";
constexpr const char *kAnsiReset   = "\033[0m";

__attribute__((format(printf, 3, 4))) void fprintf_color(FILE       *f,
                                                         const char *color,
                                                         const char *fmt, ...);