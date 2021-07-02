#pragma once

#include <string>

#include "../util/getEmptyString.h"
#include "../util/stringToChars.h"

using namespace std;

// *Word(s) function implementations
namespace ts {
	namespace sl {
		void* firstWord(size_t argc, void** args) {
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

		void* restWords(size_t argc, void** args) {
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

		void* getWord(size_t argc, void** args) {
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

		void* getWords(size_t argc, void** args) {
			if(argc >= 3) {
				const char* words = (const char*)args[0];
				int startCount = *((double*)args[1]);
				int endCount = *((double*)args[2]);
				string output;
				int spaceCount = 0;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						if(spaceCount >= startCount && spaceCount < endCount) {
							output += character;
						}
						spaceCount++;
					}
					else if(spaceCount >= startCount && spaceCount <= endCount) {
						output += character;
					}
				}
				return stringToChars(output);
			}

			return getEmptyString();
		}

		void* getWordCount(size_t argc, void** args) {
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

		void* removeWord(size_t argc, void** args) {
			if(argc >= 2) {
				const char* words = (const char*)args[0];
				int count = *((double*)args[1]);
				string output;
				int spaceCount = 0;
				for(; *words; words++) {
					char character = *words;
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;
					}
					
					if(spaceCount != count) {
						output += character;
					}
				}
				return stringToChars(output);
			}

			return getEmptyString();
		}

		void* setWord(size_t argc, void** args) {
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
				return stringToChars(output);
			}

			return getEmptyString();
		}
	}
}