#include "simObject.h"

#include <string.h>

#include "../util/cloneString.h"
#include "../util/collapseEscape.h"
#include "../engine/engine.h"
#include "../util/numberToHex.h"
#include "../util/stringToChars.h"

namespace ts {
	namespace sl {
		Entry* strLen(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				return new Entry(string(args[0].stringData).length());
			}
			return nullptr;
		}

		Entry* getSubStr(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 3) {
				string substr = string(args[0].stringData).substr(args[1].numberData, args[2].numberData);
				return new Entry(stringToChars(substr));
			}
			return nullptr;
		}

		Entry* strPos(Engine* engine, unsigned int argc, Entry* args) {
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

		Entry* trim(Engine* engine, unsigned int argc, Entry* args) {
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

		Entry* ltrim(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				bool foundFirst = false;
				size_t firstIndex = 0;
				char character;
				for(; (character = *words); words++) {
					if(
						character != ' '
						&& character != '\t'
						&& character != '\n'
						&& character != '\r'
					) {
						foundFirst = true;
					}

					if(!foundFirst) {
						firstIndex++;
					}
				}

				string trimmed(&args[0].stringData[firstIndex]);
				return new Entry(stringToChars(trimmed));
			}

			return nullptr;
		}

		Entry* rtrim(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				bool foundFirst = false;
				size_t secondIndex = 0, length = 0;
				char character;
				for(; (character = *words); words++) {
					if(
						character == ' '
						|| character == '\t'
						|| character == '\n'
						|| character == '\r'
					) {
						foundFirst = true;
						secondIndex++;
					}
					else {
						secondIndex = 0;
					}

					length++;
				}

				if(!foundFirst) {
					secondIndex = 0;
				}

				string trimmed(args[0].stringData, length - secondIndex);
				return new Entry(stringToChars(trimmed));
			}
			
			return nullptr;
		}

		Entry* strCmp(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 2) {
				return new Entry(strcmp(args[0].stringData, args[1].stringData));
			}

			return new Entry(0.0);
		}

		Entry* strICmp(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 2) {
				return new Entry(strcasecmp(args[0].stringData, args[1].stringData));
			}

			return new Entry(0.0);
		}

		Entry* strLwr(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				string output;
				for(; *words; words++) {
					output += tolower(*words);
				}
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* strUpr(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				string output;
				for(; *words; words++) {
					output += toupper(*words);
				}
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* strChr(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 2) {
				const char search = args[1].stringData[0];
				if(!search) {
					return nullptr;
				}
	
				char* result = strchr(args[0].stringData, search); // this is freed at some point(?)
				if(result) {
					return new Entry(cloneString(result));
				}
			}

			return nullptr;
		}

		Entry* stripChars(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 2) {
				string output;
				const char* source = args[0].stringData;

				char sourceChar, replaceChar;
				while((sourceChar = *source)) {
					const char* replaceChars = args[1].stringData;
					while((replaceChar = *replaceChars)) {
						if(sourceChar == replaceChar) {
							goto increment;
						}
						replaceChars++;
					}

					output += sourceChar;
					
					increment:
					source++;
				}

				return new Entry(stringToChars(output));
			}
			
			return nullptr;
		}

		Entry* _collapseEscape(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				string value(args[0].stringData);
				string collapsed = collapseEscape(value);
				return new Entry(stringToChars(collapsed));
			}

			return nullptr;
		}

		Entry* expandEscape(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 1) {
				const char* lookup[18] = {
					"",			// 0x00
					"\\c0", // 0x01 black
					"\\c1", // 0x02 gray
					"\\c2", // 0x03 white
					"\\c3", // 0x04 red
					"\\c4", // 0x05 green
					"\\c5", // 0x06 yellow
					"\\c6", // 0x07 blue
					"\\b",	// 0x08
					"\\t",	// 0x09
					"\\n",	// 0x0A
					"\\c7",	// 0x0B purple
					"\\c8",	// 0x0C cyan
					"\\r",	// 0x0D
					"\\c9",	// 0x0E black
					"\\cr",	// 0x0F
					"\\cp",	// 0x10
					"\\co",	// 0x11
				};
				
				string output;
				const char* source = args[0].stringData;
				char character;
				for(; (character = *source); source++) {
					if(character <= 17) {
						output += lookup[(int)character];
					}
					else if(character < 32) {
						output += "\\x";
						output += numberToHex(character);
					}
					else {
						output += character;
					}
				}

				return new Entry(stringToChars(output));
			}
			
			return nullptr;
		}

		Entry* strReplace(Engine* engine, unsigned int argc, Entry* args) {
			if(argc == 3) {
				const char* words = args[0].stringData;
				const char* search = args[1].stringData;
				const char* replacement = args[2].stringData;
				size_t searchLength = strlen(search);
				string output;

				char character, replacementCharacter;
				size_t count = 0;
				for(; (character = *words); words++) {
					if((replacementCharacter = *search) == '\0') {
						output = output.erase(count - searchLength, searchLength);
						output += replacement;
						search = args[1].stringData;
						count = output.length();
					}
					else if(replacementCharacter == character) {
						search++;
					}
					else {
						search = args[1].stringData;
					}

					output += character;
					count++;
				}

				return new Entry(stringToChars(output));
			}

			return nullptr;
		}
	}
}
