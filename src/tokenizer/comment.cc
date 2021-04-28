#include "tokenizer.h"

Token Tokenizer::readComment() {
	Token token = {
		lexeme: "",
		type: COMMENT,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};
	
	// absorb "//"
	this->getChar();
	this->getChar();

	char character;
	while((character = this->getChar()) && character != '\n') {
		token.lexeme += character;
	}
	return token;
}