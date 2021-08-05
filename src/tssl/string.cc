#include "simObject.h"

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
	}
}