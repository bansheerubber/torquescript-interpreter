#include "parser.h"

Parser::Parser(Tokenizer* tokenizer) {
	// printf("parsed %d lines of code from %s\n", tokenizer->getLineCount(), tokenizer->fileName.c_str());
	this->tokenizer = tokenizer;
	this->sourceFile = new SourceFile();
	Component::ParseBody(this->sourceFile, this->tokenizer, this);
	// printf("%s\n", this->sourceFile->print().c_str());
}

Token Parser::expectToken(TokenType type1, TokenType type2, TokenType type3, TokenType type4, TokenType type5) {
	bool foundType = false;
	Token token = this->tokenizer->getToken();
	
	if(token.type == type1) {
		foundType = true;
	}
	else if(token.type == type2 && type2 != INVALID) {
		foundType = true;
	}
	else if(token.type == type3 && type3 != INVALID) {
		foundType = true;
	}
	else if(token.type == type4 && type4 != INVALID) {
		foundType = true;
	}
	else if(token.type == type5 && type5 != INVALID) {
		foundType = true;
	}

	if(!foundType) {
		this->error("unexpected token '%s' found, wanted %s", token.lexeme.c_str(), this->tokenizer->typeToName(type1));
	}

	return token;
}