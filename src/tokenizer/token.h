#pragma once

#include <string>

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
	LOGICAL_NOT, // !
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
	SPC, // SPC
	TAB, // TAB
	NL, // NL
	MODULUS_ASSIGN, // %=, technically not an argument
	MODULUS, // %, technically not an argument
	STRING, // "..."
	TAGGED_STRING, // '...'
	NUMBER,
	LOCAL_VARIABLE,
	GLOBAL_VARIABLE,
	SYMBOL,
	COMMENT,
	MEMBER_CHAIN,
};

struct Token {
	string lexeme;
	TokenType type;
	size_t lineNumber;
	size_t characterNumber;
};