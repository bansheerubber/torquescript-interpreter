#include "args.h"

vector<Argument> createArguments() {
	vector<Argument> output;

	output.push_back((Argument){
		name: "help",
		shortcut: "h",
		help: "Show help message"
	});

	output.push_back((Argument){
		name: "no-warnings",
		help: "Do not print warnings"
	});

	output.push_back((Argument){
		name: "test",
		hide: true
	});

	output.push_back((Argument){
		name: "overwrite-results",
		hide: true
	});

	return output;
}

Argument getArgumentFromName(vector<Argument> &arguments, string &name) {
	// not the fastest code but doesn't matter, we're dealing with like 10 arguments max
	for(Argument argument: arguments) {
		if(argument.name == name) {
			return argument;
		}
	}
	return {};
}

void printHelp(vector<Argument> &arguments, string argumentName) {
	cout << "usage: torquescript [options] [files or directories]" << endl;

	int helpPosition = 50;
	for(Argument argument: arguments) {
		string parameters = "--" + argument.name;
		if(argument.helpVariable != "") {
			parameters += " [" + argument.helpVariable + "]";
		}

		// handle shortcut
		if(argument.shortcut != "") {
			parameters = "-" + argument.shortcut;

			if(argument.helpVariable != "") {
				parameters += " [" + argument.helpVariable + "]";
			}

			parameters += ", --" + argument.name;

			if(argument.helpVariable != "") {
				parameters += " [" + argument.helpVariable + "]";
			}
		}
		
		cout << "    ";
		cout << parameters;
		for(unsigned int i = 0; i < helpPosition - parameters.length(); i++) {
			cout << " ";
		}
		cout << argument.help << endl;
	}
}

ParsedArguments parseArguments(vector<Argument> &arguments, int argc, char* argv[]) {
	ParsedArguments output = {
		argumentError: false,
	};

	Argument foundArgument;
	for(int i = 1; i < argc; i++) {
		string cliArgument(argv[i]);

		// search if the string argument matches any of the struct arguments
		if(cliArgument[0] == '-') {
			// push argument
			if(foundArgument.name != "") {
				if(foundArgument.needsInput) { // fail if we got no input when we need it
					output = (ParsedArguments){
						argumentError: true,
					};
					return output;
				}
				
				output.arguments.insert(pair<string, string>(foundArgument.name, "true"));
			}
			
			foundArgument = (Argument){};
			for(Argument argument: arguments) {
				if("--" + argument.name == cliArgument || (argument.shortcut != "" && "-" + argument.shortcut == cliArgument)) {
					foundArgument = argument;
				}
			}

			// fail if we got an unknown argument
			if(foundArgument.name == "") {
				output = (ParsedArguments){
					argumentError: true,
				};
				return output;
			}
		}
		// if we have a random string that isn't associated with an argument, treat it as an input file
		else if(foundArgument.name == "" || !foundArgument.needsInput) {
			output.files.push_back(cliArgument);
			foundArgument = (Argument){};
		}
		else {
			output.arguments.insert(pair<string, string>(foundArgument.name, cliArgument));
			foundArgument = (Argument){};
		}
	}

	// push argument
	if(foundArgument.name != "") {
		if(foundArgument.needsInput) { // fail if we got no input when we need it
			output = (ParsedArguments){
				argumentError: true,
			};
			return output;
		}
		
		output.arguments.insert(pair<string, string>(foundArgument.name, "true"));
	}

	return output;
}