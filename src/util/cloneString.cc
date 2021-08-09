#include "cloneString.h"
#include <cstring>

using namespace std;

char* cloneString(char* input) {
	const size_t size = strlen(input);
	char* output = new char[size + 1];
	// strncpy(output, input, size);
	memmove(output, input, size);
	output[size] = '\0';
	return output;
}