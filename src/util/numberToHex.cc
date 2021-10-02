#include "numberToHex.h"

#include <cmath>

string numberToHex(unsigned int number) {
	if(number == 0) {
		return "0";
	}

	string output;
	for(; number > 0; number /= 16) {
		int digit = number % 16;
		if(digit < 10) {
			output.insert(0, 1, '0' + digit);
		}
		else {
			output.insert(0, 1, 'A' + (digit - 10));
		}
	}
	return output;
}
