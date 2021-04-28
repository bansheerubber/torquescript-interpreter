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
	this->validKeywords.insert(pair<string, TokenType>("=", ASSIGN));
	this->validKeywords.insert(pair<string, TokenType>("!", LOGICAL_NOT));
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
	this->validKeywords.insert(pair<string, TokenType>("spc", SPC));
	this->validKeywords.insert(pair<string, TokenType>("tab", TAB));
	this->validKeywords.insert(pair<string, TokenType>("nl", NL));

	this->customLexeme.insert(pair<TokenType, string>(SPC, "SPC"));
	this->customLexeme.insert(pair<TokenType, string>(TAB, "TAB"));
	this->customLexeme.insert(pair<TokenType, string>(NL, "NL"));
	
	for(auto const &[argument, value]: this->validKeywords) {
		string output;
		for(unsigned int i = 0; i < argument.length(); i++) {
			if(!this->partialKeywords[i + 1]) {
				this->partialKeywords[i + 1] = new unordered_map<string, string>();
			}

			output += tolower(argument[i]);
			this->partialKeywords[i + 1]->insert(pair<string, string>(output, argument));

			if(i + 1 > this->largestPartial) {
				this->largestPartial = i + 1;
			}
		}
	}
}

bool Tokenizer::isPartialKeyword(char partial) {
	string partialString;
	partialString += tolower(partial);
	return (*this->partialKeywords[1]).find(partialString) != (*this->partialKeywords[1]).end();
}

bool Tokenizer::isPartialKeyword(string partial) {
	unsigned int length = partial.length();
	if(length > this->largestPartial) {
		return false;
	}
	return (*this->partialKeywords[length]).find(partial) != (*this->partialKeywords[length]).end();
}

string Tokenizer::getKeywordFromPartial(string partial) {
	if(partial.length() < this->largestPartial) {
		return (*this->partialKeywords[partial.length()])[partial];
	}
	return "";
}

bool Tokenizer::isArgument(string argument) {
	TokenType type = this->isValidKeyword(argument);
	return type >= PLUS && type <= COMMA;
}

bool Tokenizer::isAlphabeticalKeyword(string keyword) {
	TokenType type = this->isValidKeyword(keyword);
	return type >= RETURN && type <= NL;
}

bool Tokenizer::isAlphabeticalKeyword(TokenType type) {
	return type >= RETURN && type <= NL;
}

TokenType Tokenizer::isValidKeyword(string argument) {
	return this->validKeywords[argument];
}

string Tokenizer::getKeywordLexeme(TokenType type) {
	if(this->customLexeme[type] == "") {
		return "";
	}
	return this->customLexeme[type];
}

Token Tokenizer::readKeyword() {
	Token argument = {
		lexeme: "",
		type: INVALID,
		lineNumber: this->lineNumber,
		characterNumber: this->characterNumber,
	};

	string argumentBuffer;
	char character;
	bool finished = false;
	while(!this->isFileEOF() && (character = this->getChar())) {
		argumentBuffer += tolower(character);

		if(!this->isPartialKeyword(argumentBuffer)) {
			finished = true;
			break;
		}
	}

	// if we remove one character from the argument buffer, are we a valid argument?
	char nextChar = '\0';
	if(finished) {
		nextChar = argumentBuffer.back();
		argumentBuffer.pop_back();
		this->prevChar(); // give back last character we read
	}

	TokenType argumentType = this->isValidKeyword(argumentBuffer);
	bool isAlphabetical = this->isAlphabeticalKeyword(argumentBuffer);
	if(
		argumentType
		&& (
			!isAlphabetical
			|| (
				isAlphabetical
				&& !this->isValidVariableChar(nextChar)
			)
		)
	) {
		string customLexeme = this->getKeywordLexeme(argumentType);
		argument.lexeme = customLexeme != "" ? customLexeme : argumentBuffer;
		argument.type = argumentType;
		return argument;
	}
	// return invalid argument after rolling back characters
	else {
		// TODO generalize this special case
		if(argumentBuffer == "!$") {
			this->prevChar();
			argument.lexeme = "!";
			argument.type = LOGICAL_NOT;
			return argument;
		}
		
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
		unsigned int size = argumentBuffer.length();
		for(unsigned int i = 0; i < size; i++) {
			this->prevChar();
		}

		this->freezeKeywordTest = true;
		return argument;
	}
}