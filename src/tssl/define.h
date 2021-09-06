#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "../interpreter/entry.h"
#include "../util/toLower.h"

using namespace std;

namespace ts {
	class MethodTree;
	class Interpreter;
	class Engine;

	#define TS_FUNC(name)		Entry* (*name)(Engine* engine, size_t argc, Entry* args)
	#define TS_ARG_COUNT		16
	
	namespace sl {
		struct Function {
			entry::EntryType returnType;
			string nameSpace;
			string name;
			size_t argumentCount;
			TS_FUNC(function);
			entry::EntryType* argumentTypes;
		};

		extern vector<Function*> functions;
		extern unordered_map<string, size_t> nameToIndex;

		extern vector<MethodTree*> methodTrees;
		extern unordered_map<string, size_t> methodTreeNameToIndex;

		ts::sl::Function* FUNC_DEF(entry::EntryType returnType, TS_FUNC(functionPointer), const char* nameSpace, const char* name, size_t argumentCount, entry::EntryType* argumentTypes);
		ts::sl::Function* FUNC_DEF(entry::EntryType returnType, TS_FUNC(functionPointer), const char* name, size_t argumentCount, entry::EntryType* argumentTypes);

		MethodTree* NAMESPACE_DEF(const char* name);

		Entry* PARENT(Engine* engine, const char* methodName, size_t argc, Entry* argv, entry::EntryType* argumentTypes);
		
		void define(Engine* engine);
	}
}