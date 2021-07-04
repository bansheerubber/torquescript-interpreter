#include "numberToString.h"

char* numberToString(double number) {
	string formatted(fmt::format("{:G}", number));
	return stringToChars(formatted);
}

string numberToStdString(double number) {
	string formatted(fmt::format("{:G}", number));
	return formatted;
}
