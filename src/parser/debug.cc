#include <stdio.h>

#include "../io.h"
#include "parser.h"

void Parser::error(const char* format, ...) {
	Token token = this->tokenizer->peekToken();
	printError("%s:%d:%d: ", this->tokenizer->fileName.c_str(), token.lineNumber, token.characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	printError(format, argptr);
	va_end(argptr);

	printError("\n");

	exit(1);
}


static const char* ComponentTypeDebug[] = {
	"INVALID_STATEMENT",
	"SOURCE_FILE",
	"IF_STATEMENT",
	"ELSE_STATEMENT",
	"ELSE_IF_STATEMENT",
	"WHILE_STATEMENT",
	"FOR_STATEMENT",
	"SWITCH_STATEMENT",
	"ASSIGN_STATEMENT",
	"ACCESS_STATEMENT",
	"ARRAY_STATEMENT",
	"CALL_STATEMENT",
	"NUMBER_LITERAL",
	"STRING_LITERAL",
	"BOOLEAN_LITERAL",
	"MATH_EXPRESSION",
	"EXPRESSION",
	"POSTFIX_STATEMENT",
	"PARENT_STATEMENT",
	"RETURN_STATEMENT",
	"FUNCTION_DECLARATION",
	"DATABLOCK_DECLARATION",
	"SYMBOL_STATEMENT",
	"INHERITANCE_STATEMENT",
};

const char* Parser::typeToName(ComponentType type) {
	return ComponentTypeDebug[type];
}