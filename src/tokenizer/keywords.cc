#include "tokenizer.h"

unordered_map<int, unordered_map<string, string>*> Tokenizer::partialKeywords;
unordered_map<string, TokenType> Tokenizer::validKeywords;
unordered_map<TokenType, string> Tokenizer::customLexeme;

void Tokenizer::initializeKeywords() {
	validKeywords.insert(pair<string, TokenType>("!=", NOT_EQUAL));
	validKeywords.insert(pair<string, TokenType>("==", EQUAL));
	validKeywords.insert(pair<string, TokenType>("$=", STRING_EQUAL));
	validKeywords.insert(pair<string, TokenType>("!$=", STRING_NOT_EQUAL));
	validKeywords.insert(pair<string, TokenType>("<=", LESS_THAN_EQUAL));
	validKeywords.insert(pair<string, TokenType>(">=", GREATER_THAN_EQUAL));
	validKeywords.insert(pair<string, TokenType>("<", LESS_THAN));
	validKeywords.insert(pair<string, TokenType>(">", GREATER_THAN));
	validKeywords.insert(pair<string, TokenType>("&&", LOGICAL_AND));
	validKeywords.insert(pair<string, TokenType>("||", LOGICAL_OR));
	validKeywords.insert(pair<string, TokenType>("^^", LOGICAL_XOR));
	validKeywords.insert(pair<string, TokenType>("++", INCREMENT));
	validKeywords.insert(pair<string, TokenType>("--", DECREMENT));
	validKeywords.insert(pair<string, TokenType>("<<", SHIFT_LEFT));
	validKeywords.insert(pair<string, TokenType>(">>", SHIFT_RIGHT));
	validKeywords.insert(pair<string, TokenType>("+=", PLUS_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("-=", MINUS_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("/=", SLASH_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("*=", ASTERISK_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("|=", OR_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("&=", AND_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("^=", XOR_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("<<=", SHIFT_LEFT_ASSIGN));
	validKeywords.insert(pair<string, TokenType>(">>=", SHIFT_RIGHT_ASSIGN));
	validKeywords.insert(pair<string, TokenType>("=", ASSIGN));
	validKeywords.insert(pair<string, TokenType>("!", LOGICAL_NOT));
	validKeywords.insert(pair<string, TokenType>("%", MODULUS));
	validKeywords.insert(pair<string, TokenType>("&", BITWISE_AND));
	validKeywords.insert(pair<string, TokenType>("|", BITWISE_OR));
	validKeywords.insert(pair<string, TokenType>("^", BITWISE_XOR));
	validKeywords.insert(pair<string, TokenType>("?", QUESTION_MARK));
	validKeywords.insert(pair<string, TokenType>(":", COLON));
	validKeywords.insert(pair<string, TokenType>("@", APPEND));
	validKeywords.insert(pair<string, TokenType>("~", BITWISE_NOT));
	validKeywords.insert(pair<string, TokenType>("+", PLUS));
	validKeywords.insert(pair<string, TokenType>("-", MINUS));
	validKeywords.insert(pair<string, TokenType>("*", ASTERISK));
	validKeywords.insert(pair<string, TokenType>("/", SLASH));
	validKeywords.insert(pair<string, TokenType>("::", NAMESPACE));
	validKeywords.insert(pair<string, TokenType>("(", LEFT_PARENTHESIS));
	validKeywords.insert(pair<string, TokenType>(")", RIGHT_PARENTHESIS));
	validKeywords.insert(pair<string, TokenType>("[", LEFT_BRACE));
	validKeywords.insert(pair<string, TokenType>("]", RIGHT_BRACE));
	validKeywords.insert(pair<string, TokenType>("{", LEFT_BRACKET));
	validKeywords.insert(pair<string, TokenType>("}", RIGHT_BRACKET));
	validKeywords.insert(pair<string, TokenType>(";", SEMICOLON));
	validKeywords.insert(pair<string, TokenType>(",", COMMA));
	validKeywords.insert(pair<string, TokenType>("return", RETURN));
	validKeywords.insert(pair<string, TokenType>("package", PACKAGE));
	validKeywords.insert(pair<string, TokenType>("new", NEW));
	validKeywords.insert(pair<string, TokenType>("function", FUNCTION));
	validKeywords.insert(pair<string, TokenType>("if", IF));
	validKeywords.insert(pair<string, TokenType>("else", ELSE));
	validKeywords.insert(pair<string, TokenType>("while", WHILE));
	validKeywords.insert(pair<string, TokenType>("for", FOR));
	validKeywords.insert(pair<string, TokenType>("switch", SWITCH));
	validKeywords.insert(pair<string, TokenType>("switch$", STRING_SWITCH));
	validKeywords.insert(pair<string, TokenType>("case", CASE));
	validKeywords.insert(pair<string, TokenType>("or", OR));
	validKeywords.insert(pair<string, TokenType>("default", DEFAULT));
	validKeywords.insert(pair<string, TokenType>("parent", PARENT));
	validKeywords.insert(pair<string, TokenType>("continue", CONTINUE));
	validKeywords.insert(pair<string, TokenType>("break", BREAK));
	validKeywords.insert(pair<string, TokenType>("datablock", DATABLOCK));
	validKeywords.insert(pair<string, TokenType>("true", TRUE));
	validKeywords.insert(pair<string, TokenType>("false", FALSE));
	validKeywords.insert(pair<string, TokenType>("spc", SPC));
	validKeywords.insert(pair<string, TokenType>("tab", TAB));
	validKeywords.insert(pair<string, TokenType>("nl", NL));

	customLexeme.insert(pair<TokenType, string>(SPC, "SPC"));
	customLexeme.insert(pair<TokenType, string>(TAB, "TAB"));
	customLexeme.insert(pair<TokenType, string>(NL, "NL"));

	for(auto const &[argument, value]: validKeywords) {
		string output;
		for(int i = 0; i < (int)argument.length(); i++) {
			if(!partialKeywords[i + 1]) {
				partialKeywords[i + 1] = new unordered_map<string, string>();
			}

			output += tolower(argument[i]);
			partialKeywords[i + 1]->insert(pair<string, string>(output, argument));
		}
	}
}

bool Tokenizer::isPartialKeyword(char partial) {
	string partialString;
	partialString += tolower(partial);
	return (*partialKeywords[1]).find(partialString) != (*partialKeywords[1]).end();
}

bool Tokenizer::isPartialKeyword(string partial) {
	int length = (int)partial.length();
	if(partialKeywords.find(length) == partialKeywords.end()) {
		return false;
	}
	return (*partialKeywords[length]).find(partial) != (*partialKeywords[length]).end();
}

string Tokenizer::getKeywordFromPartial(string partial) {
	if(partialKeywords[(int)partial.length()]) {
		return (*partialKeywords[(int)partial.length()])[partial];
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
	return validKeywords[argument];
}

string Tokenizer::getKeywordLexeme(TokenType type) {
	if(customLexeme[type] == "") {
		return "";
	}
	return customLexeme[type];
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
	char nextChar = '\0';
	if(finished) {
		nextChar = argumentBuffer[(int)argumentBuffer.length() - 1];
		argumentBuffer.erase(argumentBuffer.length() - 1, 1);
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
		for(int i = 0; i < (int)argumentBuffer.length(); i++) {
			this->prevChar();
		}

		this->freezeKeywordTest = true;
		return argument;
	}
}