#include "tokenizer.h"

Token Tokenizer::readStringLiteral(bool isTaggedString) {
	Token output = {
		lexeme: "",
		type: isTaggedString ? TAGGED_STRING_LITERAL : STRING_LITERAL,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
	};

	this->getChar(); // absorb "

	char character;
	bool finished = false;
	while(!this->file.eof() && (character = this->getChar())) {
		if(character == '\\') { // skip testing of next character
			output.lexeme += character;
			output.lexeme += this->getChar();
		}
		// end of the string
		else if(
			(isTaggedString && character == '\'')
			|| (!isTaggedString && character == '"')
		) {
			finished = true;
			break;
		}
		else {
			output.lexeme += character;
		}
	}

	if(!finished) {
		this->error("open string, strings must end with '\"' eventually");
	}

	return output;
}