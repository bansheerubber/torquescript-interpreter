#include "tokenizer.h"

Token Tokenizer::readComment() {
	Token token = {
		lexeme: "",
		type: COMMENT,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
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