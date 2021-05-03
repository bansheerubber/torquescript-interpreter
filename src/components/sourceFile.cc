#include "sourceFile.h"
#include "../parser/parser.h"

string SourceFile::print() {
	string output;
	for(Component* child: this->children) {
		output += child->print() + this->parser->newLine;
	}
	return output;
}

ts::InstructionReturn SourceFile::compile() {
	return {};
}
