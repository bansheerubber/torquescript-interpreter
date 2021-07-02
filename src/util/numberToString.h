#pragma once

#define FMT_HEADER_ONLY
#include "../include/fmt/include/fmt/format.h"
#include <string>

#include "stringToChars.h"

using namespace std;

char* numberToString(double number) {
	string formatted(fmt::format("{:G}", number));
	return stringToChars(formatted);
}