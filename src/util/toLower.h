#pragma once

#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

string toLower(string &input) {
	string output(input);
	transform(output.begin(), output.end(), output.begin(), [](unsigned char c){ return std::tolower(c); });
	return output;
}