#include "tokenizer.h"

void Tokenizer::initializeKeywords() {
	this->validKeywords.insert(pair<string, TokenType>("!=", NOT_EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("==", EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("$=", STRING_EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("!$=", STRING_NOT_EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("<=", LESS_THAN_EQUAL));
	this->validKeywords.insert(pair<string, TokenType>(">=", GREATER_THAN_EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("<", LESS_THAN));
	this->validKeywords.insert(pair<string, TokenType>(">", GREATER_THAN));
	this->validKeywords.insert(pair<string, TokenType>("&&", LOGICAL_AND));
	this->validKeywords.insert(pair<string, TokenType>("||", LOGICAL_OR));
	this->validKeywords.insert(pair<string, TokenType>("^^", LOGICAL_XOR));
	this->validKeywords.insert(pair<string, TokenType>("++", INCREMENT));
	this->validKeywords.insert(pair<string, TokenType>("--", DECREMENT));
	this->validKeywords.insert(pair<string, TokenType>("<<", SHIFT_LEFT));
	this->validKeywords.insert(pair<string, TokenType>(">>", SHIFT_RIGHT));
	this->validKeywords.insert(pair<string, TokenType>("+=", PLUS_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("-=", MINUS_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("/=", SLASH_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("*=", ASTERISK_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("|=", OR_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("&=", AND_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("^=", XOR_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("<<=", SHIFT_LEFT_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>(">>=", SHIFT_RIGHT_ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("=", EQUAL));
	this->validKeywords.insert(pair<string, TokenType>("!", NOT));
	this->validKeywords.insert(pair<string, TokenType>("%", MODULUS));
	this->validKeywords.insert(pair<string, TokenType>("&", BITWISE_AND));
	this->validKeywords.insert(pair<string, TokenType>("|", BITWISE_OR));
	this->validKeywords.insert(pair<string, TokenType>("^", BITWISE_XOR));
	this->validKeywords.insert(pair<string, TokenType>("?", QUESTION_MARK));
	this->validKeywords.insert(pair<string, TokenType>(":", COLON));
	this->validKeywords.insert(pair<string, TokenType>("@", APPEND));
	this->validKeywords.insert(pair<string, TokenType>("~", BITWISE_NOT));
	this->validKeywords.insert(pair<string, TokenType>("+", PLUS));
	this->validKeywords.insert(pair<string, TokenType>("-", MINUS));
	this->validKeywords.insert(pair<string, TokenType>("*", ASTERISK));
	this->validKeywords.insert(pair<string, TokenType>("/", SLASH));
	this->validKeywords.insert(pair<string, TokenType>("::", NAMESPACE));
	this->validKeywords.insert(pair<string, TokenType>("(", LEFT_PARENTHESIS));
	this->validKeywords.insert(pair<string, TokenType>(")", RIGHT_PARENTHESIS));
	this->validKeywords.insert(pair<string, TokenType>("[", LEFT_BRACE));
	this->validKeywords.insert(pair<string, TokenType>("]", RIGHT_BRACE));
	this->validKeywords.insert(pair<string, TokenType>("{", LEFT_BRACKET));
	this->validKeywords.insert(pair<string, TokenType>("}", RIGHT_BRACKET));
	this->validKeywords.insert(pair<string, TokenType>(";", SEMICOLON));
	this->validKeywords.insert(pair<string, TokenType>(",", COMMA));
	this->validKeywords.insert(pair<string, TokenType>(".", DOT));
	this->validKeywords.insert(pair<string, TokenType>("return", RETURN));
	this->validKeywords.insert(pair<string, TokenType>("package", PACKAGE));
	this->validKeywords.insert(pair<string, TokenType>("new", NEW));
	this->validKeywords.insert(pair<string, TokenType>("function", FUNCTION));
	this->validKeywords.insert(pair<string, TokenType>("if", IF));
	this->validKeywords.insert(pair<string, TokenType>("else", ELSE));
	this->validKeywords.insert(pair<string, TokenType>("while", WHILE));
	this->validKeywords.insert(pair<string, TokenType>("for", FOR));
	this->validKeywords.insert(pair<string, TokenType>("switch", SWITCH));
	this->validKeywords.insert(pair<string, TokenType>("switch$", STRING_SWITCH));
	this->validKeywords.insert(pair<string, TokenType>("case", CASE));
	this->validKeywords.insert(pair<string, TokenType>("or", OR));
	this->validKeywords.insert(pair<string, TokenType>("default", DEFAULT));
	this->validKeywords.insert(pair<string, TokenType>("parent", PARENT));
	this->validKeywords.insert(pair<string, TokenType>("continue", CONTINUE));
	this->validKeywords.insert(pair<string, TokenType>("break", BREAK));
	this->validKeywords.insert(pair<string, TokenType>("datablock", DATABLOCK));
	this->validKeywords.insert(pair<string, TokenType>("true", TRUE));
	this->validKeywords.insert(pair<string, TokenType>("false", FALSE));
	this->validKeywords.insert(pair<string, TokenType>("SPC", SPC));
	this->validKeywords.insert(pair<string, TokenType>("TAB", TAB));
	this->validKeywords.insert(pair<string, TokenType>("NL", NL));
	
	for(auto const &[argument, value]: this->validKeywords) {
		string output;
		for(int i = 0; i < (int)argument.length(); i++) {
			if(!this->partialKeywords[i + 1]) {
				this->partialKeywords[i + 1] = new map<string, string>();
			}

			output += argument[i];
			this->partialKeywords[i + 1]->insert(pair<string, string>(output, argument));
		}
	}
}

bool Tokenizer::isPartialKeyword(char partial) {
	string partialString;
	partialString += tolower(partial);
	return (*this->partialKeywords[1])[partialString] != "";
}

bool Tokenizer::isPartialKeyword(string partial) {
	if(this->partialKeywords[(int)partial.length()]) {
		return (*this->partialKeywords[(int)partial.length()])[partial] != "";
	}
	return false;
}

string Tokenizer::getKeywordFromPartial(string partial) {
	if(this->partialKeywords[(int)partial.length()]) {
		return (*this->partialKeywords[(int)partial.length()])[partial];
	}
	return "";
}

bool Tokenizer::isArgument(string argument) {
	TokenType type = this->isValidKeyword(argument);
	return type >= PLUS && type <= COMMA;
}

TokenType Tokenizer::isValidKeyword(string argument) {
	return this->validKeywords[argument];
}

Token Tokenizer::readKeyword() {
	Token argument = {
		lexeme: "",
		type: INVALID,
		lineNumber: this->getLineNumber(),
		characterNumber: this->getCharacterNumber(),
	};

	string argumentBuffer;
	char character;
	bool finished = false;
	while(this->file.good() && (character = this->getChar())) {
		argumentBuffer += tolower(character);

		if(!this->isPartialKeyword(argumentBuffer)) {
			finished = true;
			break;
		}
	}

	// if we remove one character from the argument buffer, are we a valid argument?
	if(finished) {
		argumentBuffer.erase(argumentBuffer.length() - 1, 1);
		this->prevChar(); // give back last character we read
	}

	TokenType argumentType = this->isValidKeyword(argumentBuffer);
	if(argumentType) {
		argument.type = argumentType;
		return argument;
	}
	// return invalid argument after rolling back characters
	else {
		// get argument from partial
		string keyword = this->getKeywordFromPartial(argumentBuffer);
		if(this->isArgument(keyword)) { // if we're a partial argument, then display error
			this->error("invalid argument '%s' (partial argument?)", argumentBuffer.c_str());
		}

		// special case if we run out of characters to read in the file
		int overrun = this->overrun;
		for(int i = 0; i < overrun; i++) {
			this->prevChar();
		}
		
		// give back the characters we read
		for(int i = 0; i < (int)argumentBuffer.length(); i++) {
			this->prevChar();
		}

		this->freezeKeywordTest = true;
		return argument;
	}
}