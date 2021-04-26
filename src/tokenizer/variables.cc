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
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
	};
	
	this->getChar(); // absorb modulus

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

Token Tokenizer::readGlobalVariable() {
	Token token = {
		lexeme: "",
		type: GLOBAL_VARIABLE,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
	};
	
	this->getChar(); // absorb $

	bool checkFirstChar = true;
	char character;
	while((character = this->getChar())) {
		if(checkFirstChar) {
			if(this->isValidVariableFirstChar(character)) {
				token.lexeme += character;
				checkFirstChar = false;
			}
			else {
				this->error("invalid first character '%c' of global variable (must be [A-Za-z_])", character, token.lexeme.c_str());
			}
		}
		else if(this->isValidVariableChar(character)) {
			token.lexeme += character;
		}
		// check for namespace symbol
		else if(character == ':') {
			if(this->getChar() == ':') { // next character must be a namespace
				token.lexeme += "::";
				checkFirstChar = true; // each symbol delimited by namespace should be a valid variable name

				// make sure next character is valid variable symbol
				character = this->getChar();
				if(this->isValidVariableFirstChar(character)) {
					this->prevChar(); // check satisfied, give character back
				}
				else {
					this->error("invalid first character '%c' of global variable (must be [A-Za-z_])", character, token.lexeme.c_str());
				}
			}
			else {
				token.lexeme += ":";
				this->warning("incomplete namespace of global variable (namespace must be '::')", token.lexeme.c_str());
			}
		}
		else {
			this->prevChar();
			break; // we're done reading the variable name
		}
	}

	return token;
}

Token Tokenizer::readMemberChain() {
	Token token = {
		lexeme: ".",
		type: MEMBER_CHAIN,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
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