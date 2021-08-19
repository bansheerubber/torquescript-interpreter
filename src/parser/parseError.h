#pragma once

#include <exception>
#include <string>

using namespace std;

enum ParseErrorRecoverType {
	NO_RECOVER,
	SEMICOLON_RECOVER,
};

class ParseError : public exception {
	public:
		ParseError();	
		ParseError(string message, class Parser* parser, ParseErrorRecoverType type);
		
		const char* what() const throw();
	
		string fileName;
		string message = "";
		size_t startLine = 0;
		size_t startCharacter = 0;
		size_t endLine = 0;
		size_t endCharacter = 0;
		ParseErrorRecoverType type = NO_RECOVER;
};
