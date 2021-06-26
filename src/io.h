#pragma once

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

using namespace std;

void printError(const char* format, ...);
void printError(const char* format, va_list &argptr);

void printWarning(const char* format, ...);
void printWarning(const char* format, va_list &argptr);