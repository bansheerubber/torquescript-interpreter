#pragma once

#include <cstring>
#include <fstream>
#include "../include/robin-map/include/tsl/robin_map.h"
#include <string>
#include <vector>

#include "../args.h"
#include "token.h"

using namespace std;
using namespace tsl;

class Tokenizer {
	public:
		Tokenizer(string fileName, ParsedArguments args);
		Tokenizer(string piped, bool isPiped, ParsedArguments args);
		Token& getToken(bool whitespace = false);
		Token& unGetToken(bool whitespace = false);
		Token& peekToken(int offset = 0, bool whitespace = false);
		bool eof();
		void printToken(Token token);
		const char* typeToName(TokenType type);
		size_t getTotalLineCount();
		size_t getTotalCharacterCount();
		bool isAlphabeticalKeyword(TokenType keyword);
		string& getKeywordLexeme(TokenType type);

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
		bool isFileEOF();

		size_t lineNumber = 1;
		size_t characterNumber = 1;

		bool freezeKeywordTest = false;
		bool failedKeyword = false;
		int overrun = 0;
		int fileIndex = 0;
		int tokenIndex = 0;
		bool showWarnings = true;

		bool isNumber(char input);
		Token readNumber();
		
		vector<Token> tokens;
		char* contents = nullptr;
		size_t contentSize = 0;

		// be potential symbols, like function names, object names, etc, so when we fail a keyword we need to read a symbol
		robin_map<string, string> partialKeywords; // partial keyword tables. first int is length of partial keyword
		robin_map<char, string> partialKeywordCharacters;
		size_t largestPartial = 0;
		robin_map<string, TokenType> validKeywords; // map of valid keyword
		robin_map<TokenType, string> customLexeme;
		void initializeKeywords();
		bool isPartialKeyword(char partial);
		bool isPartialKeyword(string &partial);
		string& getKeywordFromPartial(string &partial);
		TokenType isValidKeyword(string &argument);
		bool isArgument(string &argument);
		bool isAlphabeticalKeyword(string &keyword);
		void readKeyword();

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
