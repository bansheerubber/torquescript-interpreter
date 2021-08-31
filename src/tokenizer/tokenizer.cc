#include "tokenizer.h"
#include "../io.h"

Tokenizer::Tokenizer(string piped, bool isPiped, ParsedArguments args) {
	this->handleArgs(args);
	
	this->contentSize = piped.size();
	this->contents = new char[this->contentSize];
	strcpy(this->contents, piped.c_str());

	this->tokenize();
}

Tokenizer::Tokenizer(string fileName, ParsedArguments args) {
	this->handleArgs(args);
	
	// read the file
	ifstream file = ifstream(fileName, ios::binary | ios::ate);
	this->fileName = fileName;

	if((file.rdstate() & ifstream::failbit) != 0) {
		printError("failed to read file %s\n", fileName.c_str());
	}

	// TODO this is probably insecure
	contentSize = file.tellg();
	this->contents = new char[this->contentSize];
	file.seekg(0);
	file.read(this->contents, this->contentSize);
	file.close();

	this->tokenize();
}

void Tokenizer::handleArgs(ParsedArguments args) {
	this->args = args;
	if(this->args.arguments["no-warnings"] != "") {
		this->showWarnings = false;
	}

	if(this->args.arguments["piped"] != "") {
		this->showWarnings = false;
	}
}

void Tokenizer::tokenize() {
	// initialize keyword tables
	this->initializeKeywords();

	bool lastCharacterWhitespace = false;
	char character;
	while(!this->isFileEOF() && (character = this->getChar())) {
		bool foundWhitespace = false;
		this->failedKeyword = false;
		
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
					lexeme: "%=",
					type: MODULUS_ASSIGN,
					lineNumber: this->lineNumber,
					characterNumber: this->characterNumber,
				});
			}
			// we just found a normal modulus token
			else {
				this->prevChar(); // give back first character of variable name
				this->tokens.push_back(Token {
					lexeme: "%",
					type: MODULUS,
					lineNumber: this->lineNumber,
					characterNumber: this->characterNumber,
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
				this->readComment();
			}
			else { // handle keyword
				this->prevChar();
				this->readKeyword();
			}
		}
		// member chain parsing
		else if(character == '.') {
			this->tokens.push_back(this->readMemberChain());
		}
		// read a symbol
		else if(this->isValidVariableFirstChar(character)) {
			this->prevChar();
			this->tokens.push_back(this->readSymbol());
		}
		else if(!this->isWhitespace(character)) {
			this->error("unexpected character '%c'", character);
		}
		else {
			if(!lastCharacterWhitespace) {
				this->tokens.push_back(Token {
					lexeme: " ",
					type: WHITESPACE,
					lineNumber: this->lineNumber,
					characterNumber: this->characterNumber,
				});
				lastCharacterWhitespace = true;
			}
			foundWhitespace = true;
		}

		if(!foundWhitespace) {
			lastCharacterWhitespace = false;
		}

		if(!this->failedKeyword) {
			this->freezeKeywordTest = false;
		}
	}

	this->tokens.push_back(Token {
		lexeme: "EOF",
		type: END_OF_FILE,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	});

	// print the tokens
	/*for(Token token: this->tokens) {
		this->printToken(token);
	}*/
}

Token& Tokenizer::getToken(bool whitespace) {
	if(this->tokenIndex >= this->tokens.size()) {
		return this->emptyToken;
	}

	if(!whitespace) {
		Token* token = &this->tokens[this->tokenIndex++];
		while(token->type == WHITESPACE) {
			if(this->tokenIndex >= this->tokens.size()) {
				return this->emptyToken;
			}
			
			token = &this->tokens[this->tokenIndex++];
		}
		return *token;
	}

	return this->tokens[this->tokenIndex++];
}

Token& Tokenizer::unGetToken(bool whitespace) {
	if(this->tokenIndex <= 0) {
		return this->emptyToken;
	}

	if(!whitespace) {
		Token* token = &this->tokens[--this->tokenIndex];
		while(token->type == WHITESPACE) {
			if(this->tokenIndex <= 0) {
				return this->emptyToken;
			}
			
			token = &this->tokens[--this->tokenIndex];
		}
		return *token;
	}

	return this->tokens[--this->tokenIndex];
}

Token& Tokenizer::peekToken(int offset, bool whitespace) {
	if(!whitespace) {
		Token* token = &this->tokens[this->tokenIndex];
		int index = 0, count = 0;
		while(count <= offset) {
			if(this->tokenIndex + index >= this->tokens.size()) {
				return this->emptyToken;	
			}

			token = &this->tokens[this->tokenIndex + index];			
			if(token->type != WHITESPACE) {
				count++;
			}

			index++;
		}

		return *token;
	}
	else {
		if(this->tokenIndex + offset >= this->tokens.size()) {
			return this->emptyToken;
		}

		if(this->tokenIndex + offset < 0) {
			printError("token index is somehow below 0");
		}

		return this->tokens[this->tokenIndex + offset];
	}
}

bool Tokenizer::eof() {
	if(this->tokenIndex >= this->tokens.size()) {
		return true;
	}
	return false;
}