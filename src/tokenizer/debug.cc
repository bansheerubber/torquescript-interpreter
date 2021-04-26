#include "tokenizer.h"
#include "../io.h"

void Tokenizer::error(const char* format, ...) {
	printError("%s:%d:%d: ", this->fileName.c_str(), this->lastValidLineNumber, this->lastValidCharacterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	printError(format, argptr);
	va_end(argptr);

	printError("\n");

	exit(1);
}

static const char* TokenTypeDebug[] = {
	"INVALID",
	"END_OF_FILE",
	"PLUS",
	"MINUS",
	"SLASH",
	"ASTERISK",
	"NOT_EQUAL",
	"EQUAL",
	"STRING_EQUAL",
	"STRING_NOT_EQUAL",
	"LESS_THAN_EQUAL",
	"GREATER_THAN_EQUAL",
	"LESS_THAN",
	"GREATER_THAN",
	"LOGICAL_AND",
	"LOGICAL_OR",
	"LOGICAL_XOR",
	"INCREMENT",
	"DECREMENT",
	"SHIFT_LEFT",
	"SHIFT_RIGHT",
	"PLUS_ASSIGN",
	"MINUS_ASSIGN",
	"SLASH_ASSIGN",
	"ASTERISK_ASSIGN",
	"OR_ASSIGN",
	"AND_ASSIGN",
	"XOR_ASSIGN",
	"SHIFT_LEFT_ASSIGN",
	"SHIFT_RIGHT_ASSIGN",
	"ASSIGN",
	"LOGICAL_NOT",
	"BITWISE_AND",
	"BITWISE_OR",
	"BITWISE_XOR",
	"BITWISE_NOT",
	"QUESTION_MARK",
	"COLON",
	"APPEND",
	"NAMESPACE",
	"LEFT_PARENTHESIS",
	"RIGHT_PARENTHESIS",
	"LEFT_BRACE",
	"RIGHT_BRACE",
	"LEFT_BRACKET",
	"RIGHT_BRACKET",
	"SEMICOLON",
	"COMMA",
	"DOT",
	"RETURN",
	"PACKAGE",
	"NEW",
	"FUNCTION",
	"IF",
	"ELSE",
	"WHILE",
	"FOR",
	"SWITCH",
	"STRING_SWITCH",
	"CASE",
	"OR",
	"DEFAULT",
	"PARENT",
	"CONTINUE",
	"BREAK",
	"DATABLOCK",
	"TRUE",
	"FALSE",
	"SPC",
	"TAB",
	"NL",
	"MODULUS_ASSIGN",
	"MODULUS",
	"STRING_LITERAL",
	"TAGGED_STRING_LITERAL",
	"NUMBER",
	"LOCAL_VARIABLE",
	"GLOBAL_VARIABLE",
	"SYMBOL",
	"COMMENT",
};

const char* Tokenizer::typeToName(TokenType type) {
	return TokenTypeDebug[type];
}

void Tokenizer::printToken(Token token) {
	printf("Token {\n");
	printf("   lexeme: %s\n", token.lexeme.c_str());
	printf("   type: %s\n", TokenTypeDebug[token.type]);
	printf("   lineNumber: %d\n", token.lineNumber);
	printf("   characterNumber: %d\n", token.characterNumber);
	printf("}\n");
}