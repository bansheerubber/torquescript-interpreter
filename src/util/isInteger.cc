#include "isInteger.h"

bool isInteger(const char* string) {
	for(; *string; string++) {
		if(!(*string >= '0' && *string <= '9')) {
			return false;
		}
	}
	return true;
}