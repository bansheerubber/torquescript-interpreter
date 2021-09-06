#include "tokenizer.h"
#include "../io.h"

#include "../engine/engine.h"

void Tokenizer::error(const char* format, ...) {
	(*this->engine->errorFunction)("%s:%ld:%ld: ", this->fileName.c_str(), this->lineNumber, this->characterNumber);
	
	va_list argptr;
	va_start(argptr, format);
	(*this->engine->vErrorFunction)(format, argptr);
	va_end(argptr);

	(*this->engine->errorFunction)("\n");

	exit(1);
}

void Tokenizer::warning(const char* format, ...) {
	if(this->showWarnings) {
		(*this->engine->warningFunction)("%s:%ld:%ld: ", this->fileName.c_str(), this->lineNumber, this->characterNumber);
		
		va_list argptr;
		va_start(argptr, format);
		(*this->engine->vWarningFunction)(format, argptr);
		va_end(argptr);

		(*this->engine->warningFunction)("\n");
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