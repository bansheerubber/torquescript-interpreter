#include "tokenizer.h"

void Tokenizer::initializeKeywords() {
	## keywords.py

	this->customLexeme.insert(pair<TokenType, string>(SPC, "SPC"));
	this->customLexeme.insert(pair<TokenType, string>(TAB, "TAB"));
	this->customLexeme.insert(pair<TokenType, string>(NL, "NL"));
	
	for(auto const &[argument, value]: this->validKeywords) {
		this->partialKeywordCharacters.insert(pair<char, string>(argument[0], argument));
		
		string output;
		for(size_t i = 0; i < argument.length(); i++) {
			output += tolower(argument[i]);
			this->partialKeywords.insert(pair<string, string>(output, argument));

			if(i + 1 > this->largestPartial) {
				this->largestPartial = i + 1;
			}
		}
	}
}

bool Tokenizer::isPartialKeyword(char partial) {
	return this->partialKeywordCharacters.find(tolower(partial)) != this->partialKeywordCharacters.end();
}

bool Tokenizer::isPartialKeyword(string &partial) {
	size_t length = partial.length();
	if(length > this->largestPartial) {
		return false;
	}
	return this->partialKeywords.find(partial) != this->partialKeywords.end();
}

string& Tokenizer::getKeywordFromPartial(string &partial) {
	static string emptyString = "";
	
	auto result = this->partialKeywords.find(partial);
	if(partial.length() < this->largestPartial && result != this->partialKeywords.end()) {
		return result.value();
	}
	return emptyString;
}

bool Tokenizer::isArgument(string &argument) {
	TokenType type = this->isValidKeyword(argument);
	return type >= PLUS && type <= COMMA;
}

bool Tokenizer::isAlphabeticalKeyword(string &keyword) {
	TokenType type = this->isValidKeyword(keyword);
	return type >= RETURN && type <= NL;
}

bool Tokenizer::isAlphabeticalKeyword(TokenType type) {
	return type >= RETURN && type <= NL;
}

TokenType Tokenizer::isValidKeyword(string &argument) {
	auto result = this->validKeywords.find(argument);
	if(result != this->validKeywords.end()) {
		return result->second;
	}
	return INVALID;
}

string& Tokenizer::getKeywordLexeme(TokenType type) {
	static string emptyString = "";
	
	auto result = this->customLexeme.find(type);
	if(result != this->customLexeme.end()) {
		return result.value();
	}
	return emptyString;
}

void Tokenizer::readKeyword() {
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
		this->tokens.push_back(argument);
	}
	// return invalid argument after rolling back characters
	else {
		// TODO generalize this special case
		if(argumentBuffer == "!$") {
			this->prevChar();
			argument.lexeme = "!";
			argument.type = LOGICAL_NOT;
			this->tokens.push_back(argument);
			return;
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
		size_t size = argumentBuffer.length();
		for(size_t i = 0; i < size; i++) {
			this->prevChar();
		}

		this->freezeKeywordTest = true;
		this->failedKeyword = true;
	}
}