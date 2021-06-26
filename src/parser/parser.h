#pragma once

#include <unordered_map>
#include <string>

#include "../args.h"
#include "../tokenizer/tokenizer.h"
#include "../components/component.h"
#include "../components/sourceFile.h"

using namespace std;

class Parser {
	public:
		Parser(Tokenizer* tokenizer, ParsedArguments args);
		void error(const char* format, ...);
		
		// there's no clean way to do what i want to do here
		Token expectToken(TokenType, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID);

		const char* typeToName(ComponentType type);

		SourceFile* getSourceFile();

		string printJSON();

		string newLine;
		string tab;
		string space;

		bool minified = false;

		unordered_map<string, relative_stack_location> variableToLocation; // TODO remove this
		relative_stack_location nextLocation = 0;

		Tokenizer* tokenizer;
	
	private:
		SourceFile* sourceFile;
		ParsedArguments args;
};