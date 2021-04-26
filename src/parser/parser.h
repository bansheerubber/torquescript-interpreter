#pragma once

#include <stdarg.h>

#include "../tokenizer/tokenizer.h"
#include "./components/component.h"
#include "./components/sourceFile.h"

class Parser {
	public:
		Parser(Tokenizer* tokenizer);
		void error(const char* format, ...);
		
		// there's no clean way to do what i want to do here
		Token expectToken(TokenType, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID);

		const char* typeToName(ComponentType type);

		vector<Component*> orphans;
	
	private:
		Tokenizer* tokenizer;
		SourceFile* sourceFile;
};