#include "body.h"

string Body::printBody() {
	string output;
	for(Component* child: this->children) {
		string childPrint = child->print();

		if(childPrint.find('\n') == string::npos) {
			output += "\t" + childPrint + "\n";
		}
		else { // add tabs to all lines of child's print
			stringstream stream(childPrint);
			string item;
			vector<string> elems;
			while(getline(stream, item)) {
				output += "\t" + item + "\n";
			}
		}
	}
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