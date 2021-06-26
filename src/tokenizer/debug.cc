#include "tokenizer.h"
#include "../io.h"

void Tokenizer::error(const char* format, ...) {
	printError("%s:%ld:%ld: ", this->fileName.c_str(), this->lineNumber, this->characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	printError(format, argptr);
	va_end(argptr);

	printError("\n");

	exit(1);
}

void Tokenizer::warning(const char* format, ...) {
	if(this->showWarnings) {
		printWarning("%s:%ld:%ld: ", this->fileName.c_str(), this->lineNumber, this->characterNumber);
		
		va_list argptr;
		va_start(argptr, format);
		printWarning(format, argptr);
		va_end(argptr);

		printWarning("\n");
	}
}

## token_debug.py

const char* Tokenizer::typeToName(TokenType type) {
	return TokenTypeDebug[type];
}

void Tokenizer::printToken(Token token) {
	printf("Token {\n");
	printf("   lexeme: %s\n", token.lexeme.c_str());
	printf("   type: %s\n", TokenTypeDebug[token.type]);
	printf("   lineNumber: %ld\n", token.lineNumber);
	printf("   characterNumber: %ld\n", token.characterNumber);
	printf("}\n");
}