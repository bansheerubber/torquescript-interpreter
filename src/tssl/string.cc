#include "simObject.h"

#include <string.h>

#include "../interpreter/interpreter.h"
#include "../util/stringToChars.h"

namespace ts {
	namespace sl {
		Entry* strLen(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				return new Entry(string(args[0].stringData).length());
			}
			return nullptr;
		}

		Entry* getSubStr(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 3) {
				string substr = string(args[0].stringData).substr(args[1].numberData, args[2].numberData);
				return new Entry(stringToChars(substr));
			}
			return nullptr;
		}

		Entry* strPos(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				size_t position = string(args[0].stringData).find(args[1].stringData);
				if (position == std::string::npos) {
					return new Entry(-1);
				}
				else {
					return new Entry(position);
				}
			}
			else if(argc == 3) {
				size_t position = string(args[0].stringData).find(args[1].stringData, args[2].numberData);
				if (position == std::string::npos) {
					return new Entry(-1);
				}
				else {
					return new Entry(position);
				}
			}

			return new Entry(-1);
		}

		Entry* trim(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				bool foundFirst = false;
				size_t firstIndex = 0, secondIndex = 0, length = 0;
				for(; *words; words++) {
					char character = *words;

					if(
						character == ' '
						|| character == '\t'
						|| character == '\n'
						|| character == '\r'
					) {
						secondIndex++;
					}
					else {
						foundFirst = true;
						secondIndex = 0;
					}

					if(!foundFirst) {
						firstIndex++;
					}

					length++;
				}

				if(!foundFirst) {
					secondIndex = 0;
				}

				string trimmed(&args[0].stringData[firstIndex], length - firstIndex - secondIndex);
				return new Entry(stringToChars(trimmed));
			}

			return nullptr;
		}

		Entry* strCmp(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				return new Entry(strcmp(args[0].stringData, args[1].stringData));
			}

			return new Entry(0.0);
		}

		Entry* strICmp(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2) {
				return new Entry(strcasecmp(args[0].stringData, args[1].stringData));
			}

			return new Entry(0.0);
		}
	}
}