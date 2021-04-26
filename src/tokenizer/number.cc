#include "tokenizer.h"

bool Tokenizer::isNumber(char input) {
	if(input >= '0' && input <= '9') {
		return true;
	}
	else {
		return false;
	}
}

Token Tokenizer::readNumber() {
	Token number = {
		lexeme: "",
		type: NUMBER,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
	};
	
	char character;
	while((character = this->getChar())) {
		if(this->isNumber(character)) {
			number.lexeme += character;
		}
		// handle exponents
		else if(character == 'e' || character == 'E') {
			number.lexeme += character;

			character = this->getChar();
			if(this->isNumber(character) || character == '-') {
				number.lexeme += character;
			}
			else {
				this->error("invalid character '%c' after exponent point (number or negative sign must follow exponent symbol)", character);
			}
		}
		// if this character is a decimal, we need the next character to be a number
		else if(character == '.') {
			number.lexeme += character;

			character = this->getChar();
			if(this->isNumber(character)) {
				number.lexeme += character;
			}
			else {
				this->error("invalid character '%c' after decimal point (number must follow decimal point)", character);
			}
		}
		else {
			this->prevChar();
			break;
		}
	}

	return number;
}