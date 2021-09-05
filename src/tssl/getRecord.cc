#include "getRecord.h"

#include <string>

#include "../engine/engine.h"
#include "../util/getEmptyString.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		Entry* firstRecord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				## tokenizing.py first "\n" args[0].stringData
				return new Entry(stringToChars(first));
			}

			return nullptr;
		}

		Entry* restRecords(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 1) {
				## tokenizing.py rest "\n" args[0].stringData
				return new Entry(stringToChars(rest));
			}

			return nullptr;
		}

		Entry* getRecord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py getSingle "\n" args[0].stringData args[1].numberData
				return new Entry(stringToChars(word));
			}

			return nullptr;
		}

		Entry* getRecords(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py getMultiple "\n" args[0].stringData args[1].numberData args[2].numberData
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* getRecordCount(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				## tokenizing.py getCount "\n" args[0].stringData
				return new Entry((double)count);
			}

			return new Entry((double)0);
		}

		Entry* removeRecord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 2) {
				## tokenizing.py remove "\n" args[0].stringData args[1].numberData

				// for some reason torquescript returns the last word if count > final space count
				if(count > spaceCount) {
					return new Entry(stringToChars(currentWord));
				}

				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* setRecord(Engine* engine, size_t argc, Entry* args) {
			if(argc >= 3) {
				## tokenizing.py set "\n" args[0].stringData args[1].numberData args[2].stringData
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}
	}
}