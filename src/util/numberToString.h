#pragma once

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <string>

using namespace std;

string* numberToString(double number) {
	return new string(fmt::format("{:G}", number));
}