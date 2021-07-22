#include "sourceFile.h"
#include "../parser/parser.h"
#include "../interpreter/interpreter.h"

#include "../interpreter/entry.h"
#include "../util/getEmptyString.h"

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

ts::InstructionReturn SourceFile::compile(ts::Interpreter* interpreter, ts::CompilationContext context) {
	ts::InstructionReturn output;

	// compile source file
	for(Component* child: this->children) {
		ts::InstructionReturn compiled = child->compile(interpreter, (ts::CompilationContext){
			loop: nullptr,
			package: nullptr,
			scope: this,
		});
		if(compiled.first != nullptr) {
			output.add(compiled);
		}
	}

	for(auto const& [key, value]: this->variables) {
		ts::Instruction* push = new ts::Instruction();
		push->type = ts::instruction::PUSH;
		push->push.entry = ts::Entry();
		push->push.entry.setString(getEmptyString());
		output.addFirst(push);
	}

	output.addFirst(this->compileLinkVariables(interpreter));

	return output;
}