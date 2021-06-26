#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct Argument {
	string name;
	string shortcut;
	string helpVariable;
	string help;
	bool needsInput;
	bool hide;
};

struct ParsedArguments {
	map<string, string> arguments;
	vector<string> files;
	bool argumentError;
};

vector<Argument> createArguments();
Argument getArgumentFromName(vector<Argument> &arguments, string &name);
void printHelp(vector<Argument> &arguments, string argumentName = "");
ParsedArguments parseArguments(vector<Argument> &arguments, int argc, char* argv[]);