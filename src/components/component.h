#pragma once

#include <string>

#include "componentTypeEnum.h"
#include "../compiler/context.h"
#include "../engine/engine.h"
#include "../tokenizer/tokenizer.h"
#include "../interpreter/instruction.h"

using namespace std;

class Component {
	public:
		Component(ts::Engine* engine);
		virtual ~Component() {}
		
		virtual ComponentType getType() = 0; // gets the type of the component
		virtual string print() = 0; // prints valid torquescript
		virtual string printJSON() = 0; // serializes to JSON
		virtual bool requiresSemicolon(Component* child) = 0; // whether or not a child of this component needs a semicolon
		virtual bool shouldPushToStack(Component* child) = 0; // whether or not a child should push its value to the stack
		virtual ts::InstructionReturn compile(ts::Engine* engine, ts::CompilationContext context) = 0; // compile to bytecode

		static bool ShouldParse(Component* parent, ts::Engine* engine);
		static Component* AfterParse(Component* lvalue, Component* parent, ts::Engine* engine);
		static Component* Parse(Component* parent, ts::Engine* engine);
		static void ParseBody(class Body* body, ts::Engine* engine, bool oneLine = false);

		void setParent(Component* parent) {
			this->parent = parent;
		}

		Component* parent = nullptr;
	
	protected:
		ts::Engine* engine = nullptr;
};
