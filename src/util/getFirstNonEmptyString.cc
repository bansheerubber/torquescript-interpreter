#include "getFirstNonEmptyString.h"

string getFirstNonEmptyString(string input1, string input2, string input3, string input4, string input5) {
	if(input1.length() != 0) {
		return input1;
	}
	else if(input2.length() != 0) {
		return input2;
	}
	else if(input3.length() != 0) {
		return input3;
	}
	else if(input4.length() != 0) {
		return input4;
	}
	else if(input5.length() != 0) {
		return input5;
	}

	string output;
	return output;
}
