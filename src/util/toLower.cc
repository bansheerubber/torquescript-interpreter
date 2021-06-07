#include "toLower.h"

string toLower(string &input) {
	string output(input);
	transform(output.begin(), output.end(), output.begin(), [](unsigned char c){ return std::tolower(c); });
	return output;
}
