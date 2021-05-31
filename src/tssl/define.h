#pragma once

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

#define TS_FUNC(name)		void (*name)(int argc, void** args)
#define TS_ARG_COUNT		16

namespace ts {
	namespace functions {
		// types for working with the C++ <-> TS interface
		enum type {
			VOID,
			STRING,
			NUMBER,
		};
		
		struct Function {
			type returnType;
			string name;
			int argumentCount;
			TS_FUNC(function);
			type* argumentTypes;
		};

		extern vector<Function*> functions;
		extern unordered_map<string, unsigned long> nameToIndex;

		void FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, int argumentCount, type* argumentTypes);
		
		void define();
	}
}