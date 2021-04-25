#pragma once

#include <fstream>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

enum TokenType {
	INVALID = 0,
	END_OF_FILE,
	PLUS, // +
	MINUS, // -
	SLASH, // /
	ASTERISK, // *
	NOT_EQUAL, // !=
	EQUAL, // ==
	STRING_EQUAL, // $=
	STRING_NOT_EQUAL, // !$=
	LESS_THAN_EQUAL, // <=
	GREATER_THAN_EQUAL, // >=
	LESS_THAN, // <
	GREATER_THAN, // >
	LOGICAL_AND, // &&
	LOGICAL_OR, // ||
	LOGICAL_XOR, // ^^
	INCREMENT, // ++
	DECREMENT, // --
	SHIFT_LEFT, // <<
	SHIFT_RIGHT, // >>
	PLUS_ASSIGN, // +=
	MINUS_ASSIGN, // -=
	SLASH_ASSIGN, // /=
	ASTERISK_ASSIGN, // *=
	OR_ASSIGN, // |=
	AND_ASSIGN, // &=
	XOR_ASSIGN, // ^=
	SHIFT_LEFT_ASSIGN, // <<=
	SHIFT_RIGHT_ASSIGN, // >>=
	ASSIGN, // =
	NOT, // !
	BITWISE_AND, // &
	BITWISE_OR, // |
	BITWISE_XOR, // ^
	BITWISE_NOT, // ~
	QUESTION_MARK, // ?
	COLON, // :
	APPEND, // @
	NAMESPACE, // ::
	LEFT_PARENTHESIS, // (
	RIGHT_PARENTHESIS, // )
	LEFT_BRACE, // [
	RIGHT_BRACE, // ]
	LEFT_BRACKET, // {
	RIGHT_BRACKET, // }
	SEMICOLON, // ;
	COMMA, // ,
	DOT, // .
	RETURN,
	PACKAGE,
	NEW,
	FUNCTION,
	IF,
	ELSE,
	WHILE,
	FOR,
	SWITCH,
	STRING_SWITCH,
	CASE,
	OR,
	DEFAULT,
	PARENT,
	CONTINUE,
	BREAK,
	DATABLOCK,
	TRUE,
	FALSE,
	SPC,
	TAB,
	NL,
	MODULUS_ASSIGN, // %=, technically not an argument
	MODULUS, // %, technically not an argument
	STRING_LITERAL, // "..."
	TAGGED_STRING_LITERAL, // '...'
	NUMBER,
	LOCAL_VARIABLE,
	GLOBAL_VARIABLE,
	SYMBOL,
	COMMENT,
};

struct Token {
	string lexeme;
	TokenType type;
	int lineNumber;
	int characterNumber;
};

struct CharacterInformation {
	int lineNumber;
	int characterNumber;
};

class Tokenizer {
	public:
		Tokenizer(string fileName);
		Token getToken();
	
	private:
		char getChar();
		void prevChar();
		void printToken(Token token);
		void error(const char* format, ...);
		bool isWhitespace(char character);
		int getLineNumber(int offset = 0);
		int getCharacterNumber(int offset = 0);
		int lineNumber = 1;
		int characterNumber = 0;
		bool freezeKeywordTest = false;
		int overrun = 0;

		bool isNumber(char input);
		Token readNumber();
		
		vector<Token> tokens;
		vector<CharacterInformation> info;
		ifstream file;
		string fileName;

		// be potential symbols, like function names, object names, etc, so when we fail a keyword we need to read a symbol
		map<int, map<string, string>*> partialKeywords; // partial keyword tables. first int is length of partial keyword
		map<string, TokenType> validKeywords; // map of valid keyword
		void initializeKeywords();
		bool isPartialKeyword(char partial);
		bool isPartialKeyword(string partial);
		string getKeywordFromPartial(string partial);
		TokenType isValidKeyword(string argument);
		bool isArgument(string argument);
		Token readKeyword();

		bool isValidVariableFirstChar(char character);
		bool isValidVariableChar(char character);
		Token readLocalVariable();
		Token readGlobalVariable();

		Token readStringLiteral(bool isTaggedString);

		Token readSymbol();

		Token readComment();
};
