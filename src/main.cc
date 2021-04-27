#include <chrono>
#include <filesystem>
#include <future>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "io.h"
#include "./tokenizer/tokenizer.h"
#include "./parser/parser.h"

using namespace std;

struct Argument {
	string name;
	string shortcut;
	string helpVariable;
	string help;
	bool needsInput;
};

vector<Argument> createArguments() {
	vector<Argument> output;

	output.push_back((Argument){
		name: "help",
		shortcut: "h",
		help: "Show help message"
	});

	output.push_back((Argument){
		name: "output",
		shortcut: "o",
		helpVariable: "directory",
		help: "Output transpiled Torquescript file(s) to the specified directory",
		needsInput: true,
	});

	output.push_back((Argument){
		name: "threads",
		shortcut: "t",
		helpVariable: "number of threads",
		help: "Number of threads used during parsing. Defaults to amount of logical cores on your computer.",
		needsInput: true,
	});

	output.push_back((Argument){
		name: "minify",
		shortcut: "m",
		help: "Minify the output files"
	});

	output.push_back((Argument){
		name: "no-warnings",
		help: "Do not print warnings"
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

void printHelp(vector<Argument> &arguments, string argumentName = "") {
	printf("usage: eggscript [options] [files or directories]\n");

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
		
		printf("    %s%*c%s\n", parameters.c_str(), (int)(helpPosition - parameters.length()), ' ', argument.help.c_str());
	}
}

struct ParsedArguments {
	map<string, string> arguments;
	vector<string> files;
	bool argumentError;
};

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
					output = {
						argumentError: true,
					};
					return output;
				}
				
				output.arguments.insert(pair<string, string>(foundArgument.name, ""));
			}
			
			foundArgument = {};
			for(Argument argument: arguments) {
				if("--" + argument.name == cliArgument || (argument.shortcut != "" && "-" + argument.shortcut == cliArgument)) {
					foundArgument = argument;
				}
			}

			// fail if we got an unknown argument
			if(foundArgument.name == "") {
				output = {
					argumentError: true,
				};
				return output;
			}
		}
		// if we have a random string that isn't associated with an argument, treat it as an input file
		else if(foundArgument.name == "" || !foundArgument.needsInput) {
			output.files.push_back(cliArgument);
			foundArgument = {};
		}
		else {
			output.arguments.insert(pair<string, string>(foundArgument.name, cliArgument));
			foundArgument = {};
		}
	}

	// push argument
	if(foundArgument.name != "") {
		if(foundArgument.needsInput) { // fail if we got no input when we need it
			output = {
				argumentError: true,
			};
			return output;
		}
		
		output.arguments.insert(pair<string, string>(foundArgument.name, ""));
	}

	return output;
}

bool isPipe() {
	// POSIX-specific code
	#ifdef __linux__
	struct stat stats;
	int result = fstat(fileno(stdin), &stats);
	if(result < 0) {
		return false;
	}

	if(S_ISFIFO(stats.st_mode) || S_ISREG(stats.st_mode)) {
		return true;
	}

	#endif
	return false;
}

void parseThread(vector<string> paths, promise<int> &&p) {
	int total = 0;
	for(string path: paths) {
		Tokenizer* tokenizer = new Tokenizer(path);
		Parser* parser = new Parser(tokenizer);
		total += tokenizer->getLineCount();
	}
	p.set_value(total);
}

int main(int argc, char* argv[]) {
	vector<Argument> arguments = createArguments();

	bool isPiped = isPipe();
	if(isPiped) {
		// TODO grab code from pipe
	}

	// parse arguments
	ParsedArguments parsed = parseArguments(arguments, argc, argv);
	if(parsed.argumentError) {
		printError("error: expected correct arguments\n\n");
		printHelp(arguments);
		return 0;
	}

	if(parsed.files.size() == 0 && !isPiped) {
		printError("error: expected files/directories\n\n");
		printHelp(arguments);
		return 0;
	}

	unsigned int maxThreadCount = thread::hardware_concurrency();
	if(parsed.arguments["threads"] != "") {
		try {
			maxThreadCount = stoi(parsed.arguments["threads"]);
			if(maxThreadCount <= 0) {
				maxThreadCount = 1;
			}
		}
		catch(...) {
			printError("error: expected integer for max thread count\n\n");
			printHelp(arguments);
			return 1;
		}
	}

	for(string fileName: parsed.files) {
		filesystem::path path(fileName);
		error_code error;

		if(filesystem::is_directory(path, error)) {
			auto start = chrono::high_resolution_clock::now();

			vector<string> paths;
			vector<thread> threads;
			vector<future<int>> futures;

			for(const auto& entry: filesystem::recursive_directory_iterator(path)) {
				string candidateFile = entry.path().string();
				if(entry.is_regular_file() && candidateFile.find(".cs") == candidateFile.length() - 3) {
					/*Tokenizer* tokenizer = new Tokenizer(candidateFile);
					Parser* parser = new Parser(tokenizer);
					total += tokenizer->getLineCount();*/
					paths.push_back(candidateFile);
				}
			}

			int increment = (int)paths.size() / maxThreadCount + (int)paths.size() % maxThreadCount;
			for(int i = 0; i < (int)paths.size(); i += increment) {
				vector<string> pathsForThread;
				for(int j = 0; j < increment && j + i < (int)paths.size(); j++) {
					pathsForThread.push_back(paths[i + j]);
				}

				promise<int> p;
				futures.push_back(move(p.get_future()));
				threads.push_back(thread(parseThread, pathsForThread, move(p)));
			}

			int totalLines = 0;
			for(int i = 0; i < (int)threads.size(); i++) {
				thread &t = threads[i];
				t.join();

				future<int> &f = futures[i];
				totalLines += f.get();
			}

			float time = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() / 1000.0;
			printf("completed parsing %d lines from %s in %.2fs\n", totalLines, path.string().c_str(), time);
		}
		else if(filesystem::is_regular_file(path, error)) {
			auto start = chrono::high_resolution_clock::now();
			
			Tokenizer* tokenizer = new Tokenizer(fileName);
			Parser* parser = new Parser(tokenizer);

			float time = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() / 1000.0;
			printf("completed parsing %d lines from %s in %.2fs\n", tokenizer->getLineCount(), path.string().c_str(), time);
		}
		else {
			printError("error opening file or directory %s\n", fileName.c_str());
		}
	}
	
	return 0;
}