#include "body.h"
#include "../parser/parser.h"

string Body::printBody() {
	string output = "";
	for(Component* child: this->children) {
		string childPrint = child->print();

		if(childPrint.find('\n') == string::npos) {
			output += this->engine->parser->tab + childPrint + this->engine->parser->newLine;
		}
		else { // add tabs to all lines of child's print
			stringstream stream(childPrint);
			string item;
			vector<string> elems;
			while(getline(stream, item)) {
				output += this->engine->parser->tab + item + this->engine->parser->newLine;
			}
		}
	}
	return output;
}

string Body::printJSONBody() {
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

void Body::addChild(Component* child) {
	this->children.push_back(child);
}

Component* Body::getLastChild() {
	if(this->children.size() == 0) {
		return nullptr;
	}
	return this->children.back();
}
