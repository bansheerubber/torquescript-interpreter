#pragma once

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

using namespace std;

int printError(const char* format, ...);
int printError(const char* format, va_list argptr);

int printWarning(const char* format, ...);
int printWarning(const char* format, va_list argptr);