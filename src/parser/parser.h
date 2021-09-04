#pragma once

#include <unordered_map>
#include <string>

#include "../args.h"
#include "../components/componentTypeEnum.h"
#include "../interpreter/stack.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

namespace ts {
	class Engine;
}

class Parser {
	public:
		Parser(ts::Engine* engine, ParsedArguments args);

		void startParse();

		void error(const char* format, ...);
		void warning(const char* format, ...);
		
		// there's no clean way to do what i want to do here
		Token expectToken(TokenType, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID, TokenType = INVALID);

		const char* typeToName(ComponentType type);

		class SourceFile* getSourceFile();

		string printJSON();

		string newLine = "\n";
		string tab = "\t";
		string space = " ";

		bool minified = false;

		unordered_map<string, relative_stack_location> variableToLocation; // TODO remove this
		relative_stack_location nextLocation = 0;
	
	private:
		ts::Engine* engine;
		class SourceFile* sourceFile;
		ParsedArguments args;
};