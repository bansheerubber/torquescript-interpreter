#include "getEmptyString.h"

char* getEmptyString() {
	char* empty = new char[1];
	empty[0] = '\0';
	return empty;
}
