#pragma once

#include <filesystem>
#include <fstream>
#include <stdarg.h>

#include "../args.h"
#include "../tokenizer/tokenizer.h"
#include "./components/component.h"
#include "./components/sourceFile.h"

using namespace std;

class Parser {
	public:
		Parser(Tokenizer* tokenizer, ParsedArguments args);
		void error(const char* format, ...);
		
		// there's no clean way to do what i want to do here
		Token expectToken(TokenType, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID);

		const char* typeToName(ComponentType type);

		vector<Component*> orphans;
		string newLine;
		string tab;
		string space;

		bool minified = false;
	
	private:
		Tokenizer* tokenizer;
		SourceFile* sourceFile;
		ParsedArguments args;
};