#include "toLower.h"

string toLower(string &input) {
	string output(input);
	transform(output.begin(), output.end(), output.begin(), [](unsigned char c){ return tolower(c); });
	return output;
}

char* toLower(char* input) {
	size_t length = strlen(input);
	char* output = new char[length + 1]{};
	for(size_t i = 0; i < length; i++) {
		output[i] = tolower(input[i]);
	}
	output[length] = '\0';
	return output;
}
