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
	bool encounteredDot = false;
	bool encounteredExponent = false;
	bool isHexadecimal = false;
	while((character = this->getChar())) {
		if(this->isNumber(character)) {
			number.lexeme += character;
		}
		// handle hexadecimal
		else if(character == 'x' || character == 'X') {
			// make sure hexadecimal format is correct
			if(number.lexeme != "0") {
				this->error("incorrect hexadecimal format");
			}
			isHexadecimal = true;
			number.lexeme += character;
		}
		// handle hexadecimal
		else if(
			isHexadecimal
			&& (
				character == 'a' || character == 'A'
				|| character == 'b' || character == 'B'
				|| character == 'c' || character == 'C'
				|| character == 'd' || character == 'D'
				|| character == 'e' || character == 'E'
				|| character == 'f' || character == 'F'
			)
		) {
			number.lexeme += character;
		}
		// handle exponents
		else if(character == 'e' || character == 'E') {
			if(encounteredExponent) {
				this->error("did not expect multiple exponents in number");
			}
			encounteredExponent = true;
			
			number.lexeme += character;

			character = this->getChar();
			if(this->isNumber(character)) {
				number.lexeme += character;
			}
			else if(character == '-' || character == '+') {
				number.lexeme += character;
				character = this->getChar();

				if(!this->isNumber(character)) {
					this->error("expected number after exponent sign");
				}
				number.lexeme += character;
			}
			else {
				this->error("invalid character '%c' after exponent point (number or sign must follow exponent symbol)", character);
			}
		}
		// if this character is a decimal, we need the next character to be a number
		else if(character == '.') {
			if(isHexadecimal) {
				this->error("did not expect decimal point in hexadecimal number");
			}
			
			if(encounteredDot) {
				if(encounteredExponent) {
					this->error("did not expect decimal point in exponent");
				}
				else {
					this->error("did not expect multiple decimal points in number");
				}
			}
			encounteredDot = true;

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