#include "sourceFile.h"
#include "../parser/parser.h"
#include "../interpreter/interpreter.h"

string SourceFile::print() {
	string output;
	for(Component* child: this->children) {
		output += child->print() + this->parser->newLine;
	}
	return output;
}

string SourceFile::printJSON() {
	string output = "[";
	string comma = this->children.size() != 1 ? "," : "";
	for(Component* child: this->children) {
		output += child->printJSON() + comma;
	}

	if(output.back() == ',') {
		output.pop_back();
	}

	output += "]";
	return output;
}

ts::InstructionReturn SourceFile::compile(ts::Interpreter* interpreter, ts::Scope* scope) {
	ts::InstructionReturn output;

	// compile source file
	for(Component* child: this->children) {
		ts::InstructionReturn compiled = child->compile(interpreter, this);
		if(compiled.first != nullptr) {
			output.add(compiled);
		}
	}

	return output;
}