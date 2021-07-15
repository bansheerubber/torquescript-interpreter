#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "../util/toLower.h"

using namespace std;

namespace ts {
	class MethodTree;
	class Interpreter;

	#define TS_FUNC(name)		void* (*name)(Interpreter* interpreter, size_t argc, void** args)
	#define TS_ARG_COUNT		16
	
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
			size_t argumentCount;
			TS_FUNC(function);
			type* argumentTypes;
		};

		extern vector<Function*> functions;
		extern unordered_map<string, size_t> nameToIndex;

		extern vector<MethodTree*> methodTrees;
		extern unordered_map<string, size_t> methodTreeNameToIndex;

		ts::sl::Function* FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, size_t argumentCount, type* argumentTypes);
		ts::sl::Function* FUNC_DEF(type returnType, TS_FUNC(functionPointer), const char* name, size_t argumentCount, type* argumentTypes);

		MethodTree* NAMESPACE_DEF(const char* name);

		void* PARENT(Interpreter* interpreter, const char* methodName, size_t argc, void** argv, sl::type* argumentTypes);
		
		void define(Interpreter* interpreter);
	}
}