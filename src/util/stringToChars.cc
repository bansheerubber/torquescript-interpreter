#include "stringToChars.h"

char* stringToChars(string &input) {
	size_t length = input.length();
	char* output = new char[length + 1];
	for(size_t i = 0; i < length; i++) {
		output[i] = input[i];
	}
	output[length] = '\0';
	return output;
}