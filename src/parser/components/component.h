#pragma once

#include <string>

#include "../../tokenizer/tokenizer.h"

using namespace std;

enum ComponentType {
	INVALID_STATEMENT,
	SOURCE_FILE,
	IF_STATEMENT,
	ELSE_STATEMENT,
	ELSE_IF_STATEMENT,
	WHILE_STATEMENT,
	FOR_STATEMENT,
	SWITCH_STATEMENT,
	CASE_STATEMENT,
	DEFAULT_STATEMENT,
	ASSIGN_STATEMENT,
	ACCESS_STATEMENT,
	ARRAY_STATEMENT,
	CALL_STATEMENT,
	NUMBER_LITERAL,
	STRING_LITERAL,
	BOOLEAN_LITERAL,
	MATH_EXPRESSION,
	POSTFIX_STATEMENT,
	PARENT_STATEMENT,
	RETURN_STATEMENT,
	FUNCTION_DECLARATION,
	DATABLOCK_DECLARATION,
	SYMBOL_STATEMENT,
	INHERITANCE_STATEMENT,
	PACKAGE_DECLARATION,
	COMMENT_STATEMENT,
	INLINE_CONDITIONAL,
	NEW_STATEMENT,
	CONTINUE_STATEMENT,
	BREAK_STATEMENT,
};

class Component {
	public:
		Component(class Parser* parser);
		
		virtual ComponentType getType() = 0; // gets the type of the component
		virtual string print() = 0; // prints valid torquescript
		virtual bool requiresSemicolon(Component* child) = 0; // whether or not a child of this component needs a semicolon

		static bool ShouldParse(Component* parent, Tokenizer* tokenizer, class Parser* parser);
		static Component* AfterParse(Component* lvalue, Component* parent, Tokenizer* tokenizer, class Parser* parser);
		static Component* Parse(Component* parent, Tokenizer* tokenizer, class Parser* parser);
		static void ParseBody(class Body* body, Tokenizer* tokenizer, class Parser* parser, bool oneLine = false);

		void setParent(Component* parent) {
			this->parent = parent;
		}
	
	protected:
		Component* parent;
		Parser* parser;
};