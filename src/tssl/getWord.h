#pragma once

#include <string>

using namespace std;

// *Word(s) function implementations
namespace ts {
	namespace sl {
		void* firstWord(int argc, void** args) {
			if(argc >= 1) {
				string* words = (string*)args[0];
				string* first = new string();
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
					if(character == ' ' || character == '\t' || character == '\n') {
						break;
					}
					*first += character;
				}
				return first;
			}

			return new string();
		}

		void* restWords(int argc, void** args) {
			if(argc >= 1) {
				string* words = (string*)args[0];
				string* rest = new string();
				bool copyDown = false;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];

					if(copyDown) {
						*rest += character;
					}

					if(character == ' ' || character == '\t' || character == '\n') {
						copyDown = true;
					}
				}
				return rest;
			}

			return new string();
		}

		void* getWord(int argc, void** args) {
			if(argc >= 2) {
				string* words = (string*)args[0];
				int count = *((double*)args[1]);
				string* word = new string();
				int spaceCount = 0;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;
					}
					else if(spaceCount == count) {
						*word += character;
					}
				}
				return word;
			}

			return new string();
		}

		void* getWords(int argc, void** args) {
			if(argc >= 3) {
				string* words = (string*)args[0];
				int startCount = *((double*)args[1]);
				int endCount = *((double*)args[2]);
				string* output = new string();
				int spaceCount = 0;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
					if(character == ' ' || character == '\t' || character == '\n') {
						if(spaceCount >= startCount && spaceCount < endCount) {
							*output += character;
						}
						spaceCount++;
					}
					else if(spaceCount >= startCount && spaceCount <= endCount) {
						*output += character;
					}
				}
				return output;
			}

			return new string();
		}

		void* getWordCount(int argc, void** args) {
			if(argc == 1) {
				string* words = (string*)args[0];
				double* count = new double(0);

				bool foundLast = false;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
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

		void* removeWord(int argc, void** args) {
			if(argc >= 2) {
				string* words = (string*)args[0];
				int count = *((double*)args[1]);
				string* output = new string();
				int spaceCount = 0;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;
					}
					
					if(spaceCount != count) {
						*output += character;
					}
				}
				return output;
			}

			return new string();
		}

		void* setWord(int argc, void** args) {
			if(argc >= 3) {
				string* words = (string*)args[0];
				int count = *((double*)args[1]);
				string* replace = (string*)args[2];
				string* output = new string();
				int spaceCount = 0;
				bool replaced = false;
				for(long unsigned int i = 0; i < words->length(); i++) {
					char character = (*words)[i];
					if(character == ' ' || character == '\t' || character == '\n') {
						spaceCount++;

						if(spaceCount == count) {
							*output += character;
						}
					}

					if(spaceCount != count) {
						*output += character;
					}
					else if(replaced == false) {
						replaced = true;
						*output += *replace;
					}
				}
				return output;
			}

			return new string();
		}
	}
}