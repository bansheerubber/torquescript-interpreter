#include "tokenizer.h"
#include "../io.h"

Tokenizer::Tokenizer(string fileName) {
	// initialize keyword tables
	this->initializeKeywords();
	
	// read the file
	this->file = ifstream(fileName);
	this->fileName = fileName;

	if((file.rdstate() & ifstream::failbit) != 0) {
		printError("failed to read file %s\n", fileName.c_str());
	}

	char character;
	while((character = this->getChar()) && this->file.good()) {
		bool failedKeyword = false;
		
		// read a number
		if(this->isNumber(character)) {
			this->prevChar();
			this->tokens.push_back(this->readNumber());
		}
		// handle modulus differently than normal arguments
		else if(character == '%') {
			// if the next character is the start of a valid variable name, then read a variable.
			// note: this guarentees that variables are at least one character long, which is good enough
			// for declaring a variable
			char nextCharacter = this->getChar();
			if(this->isValidVariableFirstChar(nextCharacter)) {
				this->prevChar(); // give back first character of variable name
				this->prevChar(); // give back modulus
				this->tokens.push_back(this->readLocalVariable());
			}
			// we found modulus assign
			else if(nextCharacter == '=') {
				this->tokens.push_back(Token {
					lexeme: "",
					type: MODULUS_ASSIGN,
					lineNumber: this->getLineNumber(2),
					characterNumber: this->getCharacterNumber(2),
				});
			}
			// we just found a normal modulus token
			else {
				this->prevChar(); // give back first character of variable name
				this->tokens.push_back(Token {
					lexeme: "",
					type: MODULUS,
					lineNumber: this->getLineNumber(1),
					characterNumber: this->getCharacterNumber(1),
				});
			}
		}
		// handle string/tagged string
		else if(character == '"' || character == '\'') {
			this->prevChar();
			this->tokens.push_back(this->readStringLiteral(character == '\''));
		}
		// read a keyword
		else if(!this->freezeKeywordTest && this->isPartialKeyword(character)) {
			char nextCharacter = this->getChar();
			this->prevChar();
			if(character == '$' && this->isValidVariableFirstChar(nextCharacter)) { // handle global variable
				this->prevChar();
				this->tokens.push_back(this->readGlobalVariable());
			}
			else if(character == '/' && nextCharacter == '/') { // handle comment
				this->prevChar();
				this->tokens.push_back(this->readComment());
			}
			else { // handle keyword
				this->prevChar();
				Token keyword = this->readKeyword();

				if(keyword.type != INVALID) {
					this->tokens.push_back(keyword);
				}
				else {
					failedKeyword = true;
				}	
			}
		}
		// read a symbol
		else if(this->isValidVariableFirstChar(character)) {
			this->prevChar();
			this->tokens.push_back(this->readSymbol());
		}
		else if(!this->isWhitespace(character)) {
			this->error("unexpected character '%c'", character);
		}

		if(!failedKeyword) {
			this->freezeKeywordTest = false;
		}
	}

	// print the tokens
	for(Token token: this->tokens) {
		this->printToken(token);
	}

	this->file.close();
}