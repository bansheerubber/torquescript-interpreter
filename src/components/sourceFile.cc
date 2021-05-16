#include "sourceFile.h"
#include "../parser/parser.h"
#include "accessStatement.h"
#include "assignStatement.h"
#include "../interpreter/interpreter.h"

string SourceFile::print() {
	string output;
	for(Component* child: this->children) {
		output += child->print() + this->parser->newLine;
	}
	return output;
}

ts::InstructionReturn SourceFile::compile(ts::Interpreter* interpreter) {
	ts::InstructionReturn output;

	// compile source file
	for(Component* child: this->children) {
		ts::InstructionReturn compiled = child->compile(interpreter);
		if(compiled.first != nullptr) {
			output.add(compiled);
		}
	}

	return output;
}