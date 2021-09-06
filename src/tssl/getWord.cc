#include "getWord.h"

#include <string>

#include "../engine/engine.h"
#include "../util/getEmptyString.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		Entry* firstWord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				## tokenizing.py first " \t\n" args[0].stringData
				return new Entry(stringToChars(first));
			}

			return nullptr;
		}

		Entry* restWords(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				## tokenizing.py rest " \t\n" args[0].stringData
				return new Entry(stringToChars(rest));
			}

			return nullptr;
		}

		Entry* getWord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py getSingle " \t\n" args[0].stringData args[1].numberData
				return new Entry(stringToChars(word));
			}

			return nullptr;
		}

		Entry* getWords(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py getMultiple " \t\n" args[0].stringData args[1].numberData args[2].numberData
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* getWordCount(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				## tokenizing.py getCount " \t\n" args[0].stringData
				return new Entry((double)count);
			}

			return new Entry((double)0);
		}

		Entry* removeWord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py remove " \t\n" args[0].stringData args[1].numberData

				// for some reason torquescript returns the last word if count > final space count
				if(count > spaceCount) {
					return new Entry(stringToChars(currentWord));
				}

				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* setWord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 3) {
				## tokenizing.py set " \t\n" args[0].stringData args[1].numberData args[2].stringData
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}
	}
}