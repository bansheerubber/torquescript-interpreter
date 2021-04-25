#include "io.h"

void printError(const char* format, ...) {
	fprintf(stderr, "\e[31;1m");
	
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);

	fprintf(stderr, "\e[0m");
}

void printError(const char* format, va_list &argptr) {
	fprintf(stderr, "\e[31;1m");
	vfprintf(stderr, format, argptr);
	fprintf(stderr, "\e[0m");
}