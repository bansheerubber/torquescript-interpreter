#pragma once

#include <fmt/core.h>
#include <string>

using namespace std;

string* numberToString(double number) {
	return new string(fmt::format("{:G}", number));
}