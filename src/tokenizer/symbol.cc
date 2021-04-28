#include "tokenizer.h"

Token Tokenizer::readSymbol() {
	Token token = {
		lexeme: "",
		type: SYMBOL,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};
	
	// read a symbol
	char character;
	while((character = this->getChar())) {
		if(this->isValidVariableChar(character)) {
			token.lexeme += character;
		}
		else {
			this->prevChar();
			break; // we're done reading the variable name
		}
	}

	return token;
}