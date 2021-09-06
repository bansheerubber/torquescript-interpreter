#ifdef _WIN32
#include <Windows.h>
#endif

#include "io.h"

#ifdef _WIN32
class WindowsColor {
	friend ostream& operator<<(ostream& out, WindowsColor color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.color);
		return out;
	}
	
	public:
		WindowsColor(WORD color): color(color) {}

	private:
		WORD color;
};
#endif

int printError(const char* format, ...) {
	char output[1024];
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(output, 1024, format, argptr);
	va_end(argptr);

	#ifdef _WIN32
	cout << WindowsColor(12) << output << WindowsColor(15);
	#endif

	#ifdef __linux__
	cout << "\e[91m" << output << "\e[0m";
	#endif

	#ifdef __APPLE__
	cout << "\e[91m" << output << "\e[0m";
	#endif

	return 0;
}

int printError(const char* format, va_list argptr) {
	char output[1024];
	vsnprintf(output, 1024, format, argptr);

	#ifdef _WIN32
	cout << WindowsColor(12) << output << WindowsColor(15);
	#endif

	#ifdef __linux__
	cout << "\e[91m" << output << "\e[0m";
	#endif

	#ifdef __APPLE__
	cout << "\e[91m" << output << "\e[0m";
	#endif

	return 0;
}

int printWarning(const char* format, ...) {
	char output[1024];
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(output, 1024, format, argptr);
	va_end(argptr);

	#ifdef _WIN32
	cout << WindowsColor(14) << output << WindowsColor(15);
	#endif

	#ifdef __linux__
	cout << "\e[93m" << output << "\e[0m";
	#endif

	#ifdef __APPLE__
	cout << "\e[93m" << output << "\e[0m";
	#endif

	return 0;
}

int printWarning(const char* format, va_list argptr) {
	char output[1024];
	vsnprintf(output, 1024, format, argptr);

	#ifdef _WIN32
	cout << WindowsColor(14) << output << WindowsColor(15);
	#endif

	#ifdef __linux__
	cout << "\e[93m" << output << "\e[0m";
	#endif

	#ifdef __APPLE__
	cout << "\e[93m" << output << "\e[0m";
	#endif

	return 0;
}