#include "cloneString.h"
#include <cstring>

using namespace std;

char* cloneString(const char* input) {
	const size_t size = strlen(input);
	char* output = new char[size + 1];
	strncpy(output, input, size);
	output[size] = '\0';
	return output;
}