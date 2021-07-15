#include "getWord.h"

#include <string>

#include "../util/getEmptyString.h"
#include "../interpreter/interpreter.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		void* firstWord(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 1) {
				const char* words = (const char*)args[0];
				string first;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						break;
					}
					first += character;
				}
				return stringToChars(first);
			}

			return getEmptyString();
		}

		void* restWords(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 1) {
				const char* words = (const char*)args[0];
				string rest;
				bool copyDown = false;
				for(; *words; words++) {
					char character = *words;

					if(copyDown) {
						rest += character;
					}

					if(character == ' ' || character == '\t' || character == '\n') {
						copyDown = true;
					}
				}
				return stringToChars(rest);
			}

			return getEmptyString();
		}

		void* getWord(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 2) {
				const char* words = (const char*)args[0];
				int count = *((double*)args[1]);
				string word;
				int spaceCount = 0;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;
					}
					else if(spaceCount == count) {
						word += character;
					}
				}
				return stringToChars(word);
			}

			return getEmptyString();
		}

		void* getWords(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 2) {
				const char* words = (const char*)args[0];
				int startCount = *((double*)args[1]);

				if(startCount < 0) {
					return getEmptyString();
				}

				int endCount = -1;
				if(argc >= 3) {
					endCount = *((double*)args[2]);
				}
				
				string output;
				int spaceCount = 0;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						if(spaceCount >= startCount && (spaceCount < endCount || endCount == -1)) {
							output += character;
						}
						spaceCount++;
					}
					else if(spaceCount >= startCount && (spaceCount <= endCount || endCount == -1)) {
						output += character;
					}
				}
				return stringToChars(output);
			}

			return getEmptyString();
		}

		void* getWordCount(Interpreter* interpreter, size_t argc, void** args) {
			if(argc == 1) {
				const char* words = (const char*)args[0];
				double* count = new double(0);

				bool foundLast = false;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						(*count)++;
						foundLast = false;
					}
					else {
						foundLast = true;
					}
				}
				*count += foundLast;
				return count;
			}

			return new double(0);
		}

		void* removeWord(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 2) {
				const char* words = (const char*)args[0];
				int count = *((double*)args[1]);
				string output;
				string currentWord;

				int spaceCount = 0;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;
						currentWord = "";
					}
					else {
						currentWord += character;
					}

					if(spaceCount != count) {
						if( // edge case for count = 0
							!(character == ' ' || character == '\t' || character == '\n')
							|| count != 0
							|| spaceCount != 1
						) {
							output += character;
						}
					}
				}

				// for some reason torquescript returns the last word if count > final space count
				if(count > spaceCount) {
					return stringToChars(currentWord);
				}

				return stringToChars(output);
			}

			return getEmptyString();
		}

		void* setWord(Interpreter* interpreter, size_t argc, void** args) {
			if(argc >= 3) {
				const char* words = (const char*)args[0];
				int count = *((double*)args[1]);
				const char* replace = (const char*)args[2];
				string output;
				int spaceCount = 0;
				bool replaced = false;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;

						if(spaceCount == count) {
							output += character;
						}
					}

					if(spaceCount != count) {
						output += character;
					}
					else if(replaced == false) {
						replaced = true;
						output += replace;
					}
				}

				// edge case for when count >= getWordCount
				for(int i = 0; i < count - spaceCount; i++) {
					output += ' ';
				}

				if(count > spaceCount) {
					output += replace;
				}

				return stringToChars(output);
			}

			return getEmptyString();
		}
	}
}