#include "collapseEscape.h"

#define INCREMENT_ESCAPE_SEQUENCE() i++;\
	if(i >= source.length()) {\
		if(shouldThrow) {\
			throw runtime_error("incomplete escape sequence in string literal");\
		}\
		break;\
	}\

string collapseEscape(string &source, bool shouldThrow) {
	string output;
	for(size_t i = 0; i < source.length(); i++) {
		if(source[i] == '\\') {
			INCREMENT_ESCAPE_SEQUENCE();
			char command = source[i];
			switch(command) {
				case 'x': {
					INCREMENT_ESCAPE_SEQUENCE();
					char hex0 = tolower(source[i]);
					int digit0 = hex0 >= 97
						? hex0 - 97 + 10
						: hex0 - 48;

					INCREMENT_ESCAPE_SEQUENCE();
					char hex1 = tolower(source[i]);
					int digit1 = hex1 >= 97
						? hex1 - 97 + 10
						: hex1 - 48;
					
					if(
						(
							digit0 < 0
							|| digit0 > 15
						)
						|| (
							digit1 < 0
							|| digit1 > 15
						)
					) {
						if(shouldThrow) {
							throw runtime_error("invalid escape sequence in string literal");
						}
					}

					char combined = 16 * digit0 + digit1;
					output += combined;
					break;
				}

				case 'c': {
					const char lookup[10] = {
						'\x01', // c0
						'\x02', // c1
						'\x03', // c2
						'\x04', // c3
						'\x05', // c4
						'\x06', // c5
						'\x07', // c6
						'\x0B', // c7
						'\x0C', // c8
						'\x0E', // c9
					};

					INCREMENT_ESCAPE_SEQUENCE();

					if(source[i] >= '0' && source[i] <= '9') {
						output += lookup[source[i] - '0'];
					}
					else if(source[i] == 'r') {
						output += '\x0F';
					}
					else if(source[i] == 'p') {
						output += '\x10';
					}
					else if(source[i] == 'o') {
						output += '\x11';
					}
					else {
						output += '\\';
						output += 'c';
						output += source[i];
					}

					break;
				}

				case 'n': {
					output += '\n';
					break;
				}

				case 'r': {
					output += '\r';
					break;
				}

				case 't': {
					output += '\t';
					break;
				}

				default: {
					output += command;
				}
			}
		}
		else {
			output += source[i];
		}
	}
	return output;
}
