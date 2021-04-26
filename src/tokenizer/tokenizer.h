#pragma once

#include <fstream>
#include <iterator>
#include <map>
#include <unordered_map>
#include <stdio.h>
#include <string>
#include <vector>

#include "token.h"

using namespace std;

struct CharacterInformation {
	int lineNumber;
	int characterNumber;
};

class Tokenizer {
	public:
		Tokenizer(string fileName);
		Token& getToken();
		Token& unGetToken();
		Token& peekToken(int offset = 0);
		bool eof();
		void printToken(Token token);
		const char* typeToName(TokenType type);
		int getLineCount();
		int getCharacterCount();

		string fileName;
	
	private:
		char getChar();
		void prevChar();
		void error(const char* format, ...);
		void warning(const char* format, ...);
		bool isWhitespace(char character);
		int getLineNumber(int offset = 0);
		int getCharacterNumber(int offset = 0);
		int lastValidLineNumber = 1;
		int lastValidCharacterNumber = 0;
		bool freezeKeywordTest = false;
		int overrun = 0;
		unsigned int fileIndex = 0;
		int tokenIndex = 0;

		bool isNumber(char input);
		Token readNumber();
		
		vector<Token> tokens;
		vector<CharacterInformation> info;
		unsigned int infoSize = 0;
		ifstream file;

		// be potential symbols, like function names, object names, etc, so when we fail a keyword we need to read a symbol
		unordered_map<int, unordered_map<string, string>*> partialKeywords; // partial keyword tables. first int is length of partial keyword
		unordered_map<string, TokenType> validKeywords; // map of valid keyword
		unordered_map<TokenType, string> customLexeme;
		string getKeywordLexeme(TokenType type);
		void initializeKeywords();
		bool isPartialKeyword(char partial);
		bool isPartialKeyword(string partial);
		string getKeywordFromPartial(string partial);
		TokenType isValidKeyword(string argument);
		bool isArgument(string argument);
		bool isAlphabeticalKeyword(string keyword);
		Token readKeyword();

		bool isValidVariableFirstChar(char character);
		bool isValidVariableChar(char character);
		Token readLocalVariable();
		Token readGlobalVariable();

		Token readStringLiteral(bool isTaggedString);

		Token readSymbol();

		Token readComment();

		Token emptyToken = {};
};
