#include "sourceFile.h"

string SourceFile::print() {
	string output;
	for(Component* child: this->children) {
		output += child->print() + "\n";
	}
	return output;
}