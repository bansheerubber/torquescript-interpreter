#pragma once

#include <string>

#include "../args.h"
#include "../interpreter/interpreter.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

namespace ts {
	class Engine {
		public:
			Engine(ParsedArguments args);

			Tokenizer* tokenizer;
			Parser* parser;
			Interpreter* interpreter;

			void interpretFile(string fileName);
			void interpretPiped(string piped);
		
		private:
			ParsedArguments args;
	};
}
