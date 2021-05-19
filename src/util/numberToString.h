#pragma once

#include <string>

using namespace std;

string* numberToString(double number) {
	return new string(to_string(number));
}