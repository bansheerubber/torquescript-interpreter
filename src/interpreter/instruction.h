#pragma once

#include "entry.h"
#include "stack.h"

namespace ts {
	namespace instruction {
		enum InstructionType {
			INVALID_INSTRUCTION, // an instruction with an invalid type will cause the interpreter to stop
			NOOP,
			PUSH, // push a literal value onto the stack, specifies type
			POP, // pop the top of the stack
			JUMP, // jump to a particular instruction
			JUMP_IF_TRUE, // jump to particular insturction if top element on stack is true, pops the element
			JUMP_IF_FALSE, // jump to particular insturction if top element on stack is false, pops the element
			MATH_ADDITION,
			MATH_SUBTRACT,
			MATH_MULTIPLY,
			MATH_DIVISION,
			MATH_MODULUS,
			MATH_SHIFT_LEFT,
			MATH_SHIFT_RIGHT,
			MATH_EQUAL,
			MATH_NOT_EQUAL,
			MATH_LESS_THAN_EQUAL,
			MATH_GREATER_THAN_EQUAL,
			MATH_LESS_THAN,
			MATH_GREATER_THAN,
			MATH_BITWISE_AND,
			MATH_BITWISE_OR,
			MATH_BITWISE_XOR,
			MATH_STRING_EQUAL,
			MATH_STRING_NOT_EQUAL,
			MATH_APPEND,
			MATH_SPC,
			MATH_TAB,
			MATH_NL,
			UNARY_MATHEMATICS, // apply a unary operator
			ARGUMENT_ASSIGN, // assign a value from the stack to a local variable, account for argument size
			LOCAL_ASSIGN_EQUAL,
			LOCAL_ASSIGN_INCREMENT,
			LOCAL_ASSIGN_DECREMENT,
			LOCAL_ASSIGN_PLUS,
			LOCAL_ASSIGN_MINUS,
			LOCAL_ASSIGN_ASTERISK,
			LOCAL_ASSIGN_SLASH,
			LOCAL_ASSIGN_MODULUS,
			LOCAL_ASSIGN_SHIFT_LEFT,
			LOCAL_ASSIGN_SHIFT_RIGHT,
			LOCAL_ASSIGN_BITWISE_AND,
			LOCAL_ASSIGN_BITWISE_XOR,
			LOCAL_ASSIGN_BITWISE_OR,
			GLOBAL_ASSIGN_EQUAL,
			GLOBAL_ASSIGN_INCREMENT,
			GLOBAL_ASSIGN_DECREMENT,
			GLOBAL_ASSIGN_PLUS,
			GLOBAL_ASSIGN_MINUS,
			GLOBAL_ASSIGN_ASTERISK,
			GLOBAL_ASSIGN_SLASH,
			GLOBAL_ASSIGN_MODULUS,
			GLOBAL_ASSIGN_SHIFT_LEFT,
			GLOBAL_ASSIGN_SHIFT_RIGHT,
			GLOBAL_ASSIGN_BITWISE_AND,
			GLOBAL_ASSIGN_BITWISE_XOR,
			GLOBAL_ASSIGN_BITWISE_OR,
			LOCAL_ACCESS, // gets the value of a local variable and puts it on the stack
			GLOBAL_ACCESS,
			CALL_FUNCTION, // call a globally scoped function
			CALL_PARENT,
			RETURN, // return from a function without returning a value
			POP_ARGUMENTS, // pop x arguments from the stack, x being obtained from the top of the stack
			CREATE_OBJECT, // create an object
			CALL_OBJECT, // call a function on an object
			OBJECT_ASSIGN_EQUAL,
			OBJECT_ASSIGN_INCREMENT,
			OBJECT_ASSIGN_DECREMENT,
			OBJECT_ASSIGN_PLUS,
			OBJECT_ASSIGN_MINUS,
			OBJECT_ASSIGN_ASTERISK,
			OBJECT_ASSIGN_SLASH,
			OBJECT_ASSIGN_MODULUS,
			OBJECT_ASSIGN_SHIFT_LEFT,
			OBJECT_ASSIGN_SHIFT_RIGHT,
			OBJECT_ASSIGN_BITWISE_AND,
			OBJECT_ASSIGN_BITWISE_XOR,
			OBJECT_ASSIGN_BITWISE_OR,
			OBJECT_ACCESS,
			LINK_VARIABLE,
			SYMBOL_ACCESS,
		};

		enum AssignOperations {
			INVALID_ASSIGN,
			EQUALS,
			INCREMENT,
			DECREMENT,
			PLUS_EQUALS,
			MINUS_EQUALS,
			ASTERISK_EQUALS,
			SLASH_EQUALS,
			MODULUS_EQUALS,
			SHIFT_LEFT_EQUALS,
			SHIFT_RIGHT_EQUALS,
			BITWISE_AND_EQUALS,
			BITWISE_XOR_EQUALS,
			BITWISE_OR_EQUALS,
		};

		enum UnaryOperator {
			INVALID_UNARY,
			BITWISE_NOT,
			LOGICAL_NOT,
			NEGATE,
		};
	}
	
	// instructions form a linked list
	struct Instruction {
		instruction::InstructionType type;
		Instruction* next; // next instruction in linked list
		size_t index; // instruction's index in flat array

		union {
			struct {
				Entry entry;
			} push;

			struct {
				union {
					Instruction* instruction;
					size_t index;
				};
			} jump;

			struct {
				union {
					Instruction* instruction;
					size_t index;
				};
				bool pop;
			} jumpIfTrue;

			struct {
				union {
					Instruction* instruction;
					size_t index;
				};
				bool pop;
			} jumpIfFalse;

			struct {
				Entry lvalueEntry;
				Entry rvalueEntry;
				int lvalueStackIndex;
				int rvalueStackIndex;
			} mathematics;

			struct {
				instruction::UnaryOperator operation; // the operator this instruction will perform
			} unaryMathematics;

			struct {
				int dimensions;
				string destination;
				size_t hash;
				bool fromStack;
				bool pushResult;
				Entry entry;
				int stackIndex;
			} localAssign;

			struct {
				int dimensions;
				string destination;
				size_t hash;
				bool fromStack;
				bool pushResult;
				Entry entry;
			} globalAssign;

			struct {
				int dimensions;
				string destination;
				size_t hash;
				bool fromStack;
				bool pushResult;
				Entry entry;
				bool popObject;
			} objectAssign;

			struct {
				int dimensions;
				string source;
				size_t hash;
				int stackIndex;
			} localAccess;

			struct {
				int dimensions;
				string source;
				size_t hash;
			} globalAccess;

			struct {
				int dimensions;
				string source;
				size_t hash;
			} objectAccess;

			struct {
				int dimensions;
				string source;
				size_t hash;
			} symbolAccess;

			struct {
				string name;
				string nameSpace;
				class PackagedFunctionList* cachedFunctionList;
				class MethodTreeEntry* cachedEntry;
				bool isCached;
			} callFunction;

			struct {
				string name;
				class MethodTreeEntry* cachedEntry;
				bool isCached;
			} callObject;

			struct {
				string typeName;
				bool typeNameCached;
				size_t methodTreeIndex;
				bool isCached; // whether or not namespaceIndex has been cached yet
				string symbolName;
				bool symbolNameCached;
			}	createObject;

			struct {
				size_t argumentCount;
			} popArguments;

			struct {
				int stackIndex;
				string source;
				size_t hash;
			} linkVariable;

			struct {
				string name;
				size_t cachedIndex;
				bool isCached;
			} callParent;
		};

		Instruction() {
			this->type = instruction::INVALID_INSTRUCTION;
			this->next = nullptr;
		}

		~Instruction() {
			
		}
	};

	void copyInstruction(Instruction &source, Instruction &destination);

	struct InstructionReturn {
		Instruction* first; // first instruction in mini linked list
		Instruction* last; // last instruction in mini linked list

		InstructionReturn() {
			this->first = nullptr;
			this->last = nullptr;
		}

		InstructionReturn(Instruction* first, Instruction* last) {
			this->first = first;
			this->last = last;
		}

		void add(Instruction* instruction) {
			if(instruction != nullptr) {
				if(this->first == nullptr) {
					this->first = instruction;
					this->last = instruction;
				}
				else {
					this->last->next = instruction;
					this->last = instruction;
				}
			}
		}

		void add(InstructionReturn compiled) {
			if(compiled.first != nullptr && compiled.last != nullptr) {
				if(this->first == nullptr) {
					this->first = compiled.first;
					this->last = compiled.last;
				}
				else {
					this->last->next = compiled.first;
					this->last = compiled.last;
				}
			}
		}

		void addFirst(Instruction* instruction) {
			if(instruction != nullptr) {
				if(this->first == nullptr) {
					this->first = instruction;
					this->last = instruction;
				}
				else {
					instruction->next = this->first;
					this->first = instruction;
				}
			}
		}

		void addFirst(InstructionReturn compiled) {
			if(compiled.first != nullptr && compiled.last != nullptr) {
				if(this->first == nullptr) {
					this->first = compiled.first;
					this->last = compiled.last;
				}
				else {
					compiled.last->next = this->first;
					this->first = compiled.first;
				}
			}
		}
	};
}