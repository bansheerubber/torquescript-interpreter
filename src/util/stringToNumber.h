#pragma once

#include <stdio.h>
#include <stdlib.h>

using namespace std;

double stringToNumber(char* &value) {
	return strtod(value, NULL);
}