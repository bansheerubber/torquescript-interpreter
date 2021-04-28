#pragma once

#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <unordered_map>
#include <stdio.h>
#include <string>
#include <vector>

#include "../args.h"
#include "token.h"

using namespace std;

struct CharacterInformation {
	int lineNumber;
	int characterNumber;
};

class Tokenizer {
	public:
		Tokenizer(string fileName, ParsedArguments args);
		Tokenizer(string piped, bool isPiped, ParsedArguments args);
		Token& getToken();
		Token& unGetToken();
		Token& peekToken(int offset = 0);
		bool eof();
		void printToken(Token token);
		const char* typeToName(TokenType type);
		int getTotalLineCount();
		int getTotalCharacterCount();
		bool isAlphabeticalKeyword(TokenType keyword);

		string fileName;
	
	private:
		ParsedArguments args;
		void handleArgs(ParsedArguments args);
		
		void tokenize();
		char getChar();
		void prevChar();
		void error(const char* format, ...);
		void warning(const char* format, ...);
		bool isWhitespace(char character);
		int getLineNumber();
		int getCharacterNumber();
		bool isFileEOF();

		unsigned int lineNumber = 1;
		unsigned int characterNumber = 1;

		bool freezeKeywordTest = false;
		int overrun = 0;
		unsigned int fileIndex = 0;
		unsigned int tokenIndex = 0;
		bool showWarnings = true;

		bool isNumber(char input);
		Token readNumber();
		
		vector<Token> tokens;
		char* contents = nullptr;
		unsigned int contentSize = 0;

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
		Token readMemberChain();

		Token readStringLiteral(bool isTaggedString);

		Token readSymbol();

		Token readComment();

		Token emptyToken = {};
};
