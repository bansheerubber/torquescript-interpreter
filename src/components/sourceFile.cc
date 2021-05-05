#include "sourceFile.h"
#include "../parser/parser.h"
#include "accessStatement.h"
#include "assignStatement.h"

string SourceFile::print() {
	string output;
	for(Component* child: this->children) {
		output += child->print() + this->parser->newLine;
	}
	return output;
}

ts::InstructionReturn SourceFile::compile() {
	ts::InstructionReturn output;

	// compile source file
	for(Component* child: this->children) {
		ts::InstructionReturn compiled = child->compile();
		if(compiled.first != nullptr) {
			if(output.first == nullptr) {
				output.first = compiled.first;
				output.last = compiled.last;
			}
			else {
				output.last->next = compiled.first;
				output.last = compiled.last;
			}
		}
	}

	return output;
}