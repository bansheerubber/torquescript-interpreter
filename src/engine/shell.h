#pragma once

#include <stdio.h>

int shellPrint(const char* format, ...);
int shellWarning(const char* format, ...);
int shellError(const char* format, ...);

int vShellPrint(const char* format, va_list &args);
int vShellWarning(const char* format, va_list &args);
int vShellError(const char* format, va_list &args);
