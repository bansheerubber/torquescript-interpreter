#include "io.h"

#ifdef _WIN32
#include <Windows.h>
#endif

void printError(const char* format, ...) {
	#ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif
	
	char output[1024];
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(output, 1024, format, argptr);
	va_end(argptr);

	cout << "\e[91m" << output << "\e[0m";
}

void printError(const char* format, va_list &argptr) {
	#ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif
	
	char output[1024];
	vsnprintf(output, 1024, format, argptr);

	cout << "\e[91m" << output << "\e[0m";
}

void printWarning(const char* format, ...) {
	#ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif
	
	char output[1024];
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(output, 1024, format, argptr);
	va_end(argptr);

	cout << "\e[93m" << output << "\e[0m";
}

void printWarning(const char* format, va_list &argptr) {
	#ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif
	
	char output[1024];
	vsnprintf(output, 1024, format, argptr);

	cout << "\e[93m" << output << "\e[0m";
}