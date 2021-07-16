#include "getWord.h"

#include <string>

#include "../util/getEmptyString.h"
#include "../interpreter/interpreter.h"
#include "../util/stringToChars.h"

using namespace std;

namespace ts {
	namespace sl {
		Entry* firstWord(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 1) {
				const char* words = args[0].stringData;
				string first;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						break;
					}
					first += character;
				}
				return new Entry(stringToChars(first));
			}

			return nullptr;
		}

		Entry* restWords(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 1) {
				const char* words = args[0].stringData;
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
				return new Entry(stringToChars(rest));
			}

			return nullptr;
		}

		Entry* getWord(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 2) {
				const char* words = args[0].stringData;
				int count = args[1].numberData;
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
				return new Entry(stringToChars(word));
			}

			return nullptr;
		}

		Entry* getWords(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 2) {
				const char* words = args[0].stringData;
				int startCount = args[1].numberData;

				if(startCount < 0) {
					return nullptr;
				}

				int endCount = -1;
				if(argc >= 3) {
					endCount = args[2].numberData;
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
				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* getWordCount(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* words = args[0].stringData;
				double count = 0;

				bool foundLast = false;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						count++;
						foundLast = false;
					}
					else {
						foundLast = true;
					}
				}
				count += foundLast;
				return new Entry((double)count);
			}

			return new Entry((double)0);
		}

		Entry* removeWord(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 2) {
				const char* words = args[0].stringData;
				int count = args[1].numberData;
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
					return new Entry(stringToChars(currentWord));
				}

				return new Entry(stringToChars(output));
			}

			return nullptr;
		}

		Entry* setWord(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc >= 3) {
				const char* words = args[0].stringData;
				int count = args[1].numberData;
				const char* replace = args[2].stringData;
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

				return new Entry(stringToChars(output));
			}

			return nullptr;
		}
	}
}