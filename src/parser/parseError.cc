#include "parseError.h"

#include "../parser/parser.h"

ParseError::ParseError() {}

ParseError::ParseError(string message, Parser* parser, ParseErrorRecoverType type) {
	this->message = message;
	this->type = type;

	Token token = parser->tokenizer->peekToken();
	this->fileName = parser->fileName;
	this->startCharacter = token.characterNumber;
	this->startLine = token.lineNumber;

	parser->errors.push_back(this);
}

const char* ParseError::what() const throw() {
	string output(this->fileName + ":" + to_string(this->startLine) + ":" + to_string(this->startCharacter) + ' ' + this->message);
	char* outputChar = new char[output.length() + 1];
	strncpy(outputChar, output.c_str(), output.length());
	outputChar[output.length()] = '\0';
	return outputChar;
}