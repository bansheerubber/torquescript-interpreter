#include "tokenizer.h"

bool Tokenizer::isValidVariableFirstChar(char character) {
	if(
		(character >= 'A' && character <= 'Z')
		|| (character >= 'a' && character <= 'z')
		|| character == '_'
	) {
		return true;
	}
	return false;
}

bool Tokenizer::isValidVariableChar(char character) {
	if(
		(character >= 'A' && character <= 'Z')
		|| (character >= 'a' && character <= 'z')
		|| (character >= '0' && character <= '9')
		|| character == '_'
	) {
		return true;
	}
	return false;
}

Token Tokenizer::readLocalVariable() {
	Token token = {
		lexeme: "",
		type: LOCAL_VARIABLE,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};
	
	this->getChar(); // absorb modulus

	char character;
	bool colonWarning = false;
	while((character = this->getChar())) {
		if(this->isValidVariableChar(character)) {
			token.lexeme += character;
		}
		else if(character == ':') {
			// peek ahead and see if we're allowed to use this token here
			char nextCharacter = this->getChar();
			this->prevChar();
			if(nextCharacter == ':' || this->isValidVariableFirstChar(nextCharacter)) {
				token.lexeme += ':';

				if(!colonWarning) {
					this->warning(": used in local variable, was it supposed to be a global variable?", token.lexeme.c_str());
					colonWarning = true;
				}
			}
			else { // if we only find one : and there's no more variables, we're probably hit the colon on a case statement
				this->prevChar();
				break;
			}
		}
		else {
			this->prevChar();
			break; // we're done reading the variable name
		}
	}

	return token;
}

Token Tokenizer::readGlobalVariable() {
	Token token = {
		lexeme: "",
		type: GLOBAL_VARIABLE,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};
	
	this->getChar(); // absorb $

	bool checkFirstChar = true;
	int colonCount = 0;
	char character;
	char lastCharacter = '\0';
	while((character = this->getChar())) {
		if(lastCharacter == ':' && character != ':') {
			if(colonCount == 1) {
				this->warning("only one : as global variable seperator", token.lexeme.c_str());
			}
			else if(colonCount > 2) {
				this->warning("more than two :'s as global variable separator", token.lexeme.c_str());
			}
			colonCount = 0;
		}
		
		if(checkFirstChar && character != ':') {
			if(this->isValidVariableFirstChar(character)) {
				token.lexeme += character;
				checkFirstChar = false;
			}
			else {
				this->error("invalid first character '%c' of global variable (must be [A-Za-z_])", character);
			}
		}
		else if(this->isValidVariableChar(character)) {
			token.lexeme += character;
		}
		// check for namespace symbol
		else if(character == ':') {
			// peek ahead and see if we're allowed to use this token here
			char nextCharacter = this->getChar();
			this->prevChar();
			if(nextCharacter == ':' || this->isValidVariableFirstChar(nextCharacter)) {
				token.lexeme += ':';
				colonCount++;
				checkFirstChar = true;
			}
			else { // if we only find one : and there's no more variables, we're probably hit the colon on a case statement
				this->prevChar();
				break;
			}
		}
		else {
			this->prevChar();
			break; // we're done reading the variable name
		}

		lastCharacter = character;
	}

	return token;
}

Token Tokenizer::readMemberChain() {
	Token token = {
		lexeme: ".",
		type: MEMBER_CHAIN,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};

	// make sure the first character is valid
	char character = this->getChar();
	if(!this->isValidVariableFirstChar(character)) {
		this->error("invalid first character '%c' of member chain (must be [A-Za-z_])", character);
	}
	token.lexeme += character;

	// read a symbol
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