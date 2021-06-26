#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "../util/toLower.h"

using namespace std;

#define TS_FUNC(name)		void* (*name)(int argc, void** args)
#define TS_ARG_COUNT		16

namespace ts {
	namespace sl {
		// types for working with the C++ <-> TS interface
		enum type {
			VOID,
			STRING,
			NUMBER,
			OBJECT,
		};
		
		struct Function {
			type returnType;
			string nameSpace;
			string name;
			int argumentCount;
			TS_FUNC(function);
			type* argumentTypes;
		};

		extern vector<Function*> functions;
		extern unordered_map<string, size_t> nameToIndex;

		void FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, int argumentCount, type* argumentTypes);
		void FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, int argumentCount, type* argumentTypes);
		
		void define();
	}
}