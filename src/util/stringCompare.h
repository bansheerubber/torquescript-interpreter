#pragma once

bool stringCompareInsensitive(char* first, char* second) {
	char* firstTemp = first;
	char* secondTemp = second;
	
	for(;; firstTemp++, secondTemp++) {
		if(tolower(*firstTemp) != tolower(*secondTemp)) {
			return false;
		}

		if(*firstTemp == '\0') { // if we're at this point, then both strings are equal
			return true;
		}
	}
}