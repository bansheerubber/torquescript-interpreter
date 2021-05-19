#pragma once

#include <string>

using namespace std;

double stringToNumber(string &value) {
	try {
		return stod(value);
	}
	catch(...) {
		return 0.0;
	}
}