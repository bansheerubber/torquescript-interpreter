#pragma once

#include <chrono>
#include <stack>
#include <vector>

#include "entry.h"
#include "instruction.h"
#include "../io.h"
#include "stack.h"
#include "variableContext.h"
#include "instructionContainer.h"
#include "function.h"
#include "../include/robin-map/include/tsl/robin_map.h"
#include "../tssl/define.h"
#include "interpreterMacros.h"
#include "../util/toLower.h"
#include "object.h"
#include "objectReference.h"
#include "../args.h"

using namespace std;

namespace ts {
	struct NamespaceFunctions {
		robin_map<string, int> nameToIndex;
		robin_map<string, Function*> nameToFunction;
		vector<Function*> functions;

		NamespaceFunctions() {
			this->nameToIndex = robin_map<string, int>();
			this->nameToFunction = robin_map<string, Function*>();
			this->functions = vector<Function*>();
		}
	};
	
	class Interpreter {
		public:
			Interpreter();
			Interpreter(ParsedArguments args);
			~Interpreter();

			void startInterpretation(Instruction* head);
			
			void printStack();
			void warning(const char* format, ...);

			void addFunction(string &name, InstructionReturn output);
			void addFunction(string &nameSpace, string &name, InstructionReturn output);
			void addTSSLFunction(sl::Function* function);

			Entry emptyEntry;

			unsigned int highestObjectId = 0;
		
		private:
			void interpret(); // interprets the next instruction

			bool warnings = true;
			
			void push(Entry &entry);
			void push(double number);
			void push(string* data);
			void push(ObjectReference* data);
			void pop();

			int ranInstructions = 0;
			chrono::high_resolution_clock::time_point startTime;

			Entry stack[1024];
			stack_location stackPointer = 0; // points to the next valid location on the stack (stackPointer - 1 == top of stack)

			void pushVariableContext();
			void popVariableContext();
			VariableContext contexts[256];
			VariableContext* topContext;
			int contextPointer = 0;
			Entry& getVariableEntry(class Instruction &instruction, string &variable, size_t hash);
			friend string VariableContext::computeVariableString(Instruction &instruction, string &variable);

			InstructionContainer* containerStack[1024];
			unsigned long containerStackPointer;
			InstructionContainer* topContainer; // the current container we're executing code from, taken from the containerStack

			unsigned long pointerStack[1024];
			unsigned long pointerStackPointer;
			unsigned long* instructionPointer; // the current instruction pointer, taken from the pointerStack
			void pushInstructionContainer(InstructionContainer* container);
			void popInstructionContainer();

			// function datastructures
			robin_map<string, int> nameToIndex;
			vector<Function*> functions;

			robin_map<string, int> namespaceToIndex;
			vector<NamespaceFunctions*> namespaceFunctions;
	};
}