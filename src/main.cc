#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <vector>

#ifdef __linux__
#include <unistd.h>
#endif

#include "args.h"
#include "./util/collapseEscape.h"
#include "./compiler/compiler.h"
#include "io.h"
#include "./interpreter/interpreter.h"
#include "./lib/lib.h"
#include "./parser/parser.h"
#include "test.h"
#include "./tokenizer/tokenizer.h"

using namespace std;

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

int main(int argc, char* argv[]) {
	vector<Argument> arguments = createArguments();

	bool isPiped = isPipe();

	// parse arguments
	ParsedArguments args = parseArguments(arguments, argc, argv);
	if(args.arguments["help"] != "") {
		printHelp(arguments);
		return 0;
	}

	// run tests if we want to
	if(args.arguments["test"] != "") {
		return runTests(args.arguments["overwrite-results"] != "");
	}

	if(args.argumentError) {
		printError("error: expected correct arguments\n\n");
		printHelp(arguments);
		return 1;
	}

	if(args.files.size() == 0 && !isPiped) {
		printError("error: expected files\n\n");
		printHelp(arguments);
		return 1;
	}

	ts::Interpreter* interpreter = new ts::Interpreter(args, false);
	
	if(isPiped) {
		string file;
		string line;
		while(getline(cin, line)) {
			file += line + "\n";
		}

		args.arguments["piped"] = "true"; // tell parser that input was piped

		Tokenizer* tokenizer = new Tokenizer(file, true, args);
		Parser* parser = new Parser(tokenizer, args);
		interpreter->startInterpretation(ts::Compile(parser, interpreter));
		delete interpreter;
	}
	else {
		for(string fileName: args.files) {
			filesystem::path path(fileName);
			error_code error;
			
			if(filesystem::is_regular_file(path, error)) {
				Tokenizer* tokenizer = new Tokenizer(fileName, args);
				Parser* parser = new Parser(tokenizer, args);
				interpreter->startInterpretation(ts::Compile(parser, interpreter));
			}
			else {
				printError("error opening file %s\n", fileName.c_str());
			}
		}

		delete interpreter;
	}
	
	return 0;
}