#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string>

using namespace std;

void printError(const char* format, ...);
void printError(const char* format, va_list &argptr);