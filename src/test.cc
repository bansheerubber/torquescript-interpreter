#include "args.h"

#include <stdlib.h>

#include "interpreter/compiler.h"
#include "tssl/echo.h"
#include "interpreter/interpreter.h"
#include "io.h"
#include "test.h"
#include "parser/parser.h"
#include "tokenizer/tokenizer.h"

bool isFileEqual(string &contents, string filename) {
	ifstream file(filename);

	string buffer;
	file.seekg(0, ios::end);
	buffer.reserve(file.tellg());
	file.seekg(0, ios::beg);
	buffer.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	return contents == buffer;
}

void printBoundary() {
	printf("=============================================================================\n");
}

// parse single file
bool parseFileTest(string name, string filename, bool overwriteResults) {
	printBoundary();
	printf("Parser test (%s): parse '%s':\n", name.c_str(), filename.c_str());

	string resultsFile(filename);
	resultsFile.replace(9, 7, "results").replace(resultsFile.find(".cs"), 3, ".json");

	ParsedArguments empty;
	Tokenizer* tokenizer = new Tokenizer(filename, empty);
	Parser* parser = new Parser(tokenizer, empty);
	string json = parser->printJSON();

	printf("parsed %ld lines\n", tokenizer->getTotalLineCount());

	if(overwriteResults) {
		filesystem::create_directories(filesystem::path(resultsFile).remove_filename());
		ofstream file;
		file.open(resultsFile);
		file << json;
		file.close();
	}

	if(isFileEqual(json, resultsFile)) {
		printf("passed test\n");
		return true;
	}
	else {
		printError("failed test\n");
		return false;
	}
}

// parse directory
void parseDirectoryTest(string filename, bool overwriteResults, int* totalTests, int* passedTests) {
	ParsedArguments empty;
	
	printBoundary();
	printf("Parser test: parse '%s':\n", filename.c_str());
	int totalLines = 0;
	for(const auto &entry: filesystem::recursive_directory_iterator(filename)) {
		string candidateFile = entry.path().string();
		if(entry.is_regular_file() && candidateFile.find(".cs") == candidateFile.length() - 3) {
			string resultsFile(candidateFile);
			resultsFile.replace(9, 7, "results").replace(resultsFile.find(".cs"), 3, ".json");

			(*totalTests)++;
			Tokenizer* tokenizer = new Tokenizer(candidateFile, empty);
			Parser* parser = new Parser(tokenizer, empty);
			string json = parser->printJSON();
			totalLines += tokenizer->getTotalLineCount();

			// overwrite old results if we need to regenerate unit test results
			if(overwriteResults) {
				filesystem::create_directories(filesystem::path(resultsFile).remove_filename());
				ofstream file;
				file.open(resultsFile);
				file << parser->printJSON();
				file.close();
			}

			if(isFileEqual(json, resultsFile)) {
				(*passedTests)++;
			}
			else {
				printError("%s failed test\n", candidateFile.c_str());
			}
		}
	}
	printf("parsed %d lines\n", totalLines);
}

// interpret directory
void interpretDirectoryTest(string filename, int* totalTests, int* passedTests) {
	ParsedArguments empty;

	ts::sl::useMockStdout = true;
	
	printBoundary();
	printf("Interpreter test: interpret '%s':\n", filename.c_str());
	int totalLines = 0;
	for(const auto &entry: filesystem::recursive_directory_iterator(filename)) {
		string candidateFile = entry.path().string();
		if(entry.is_regular_file() && candidateFile.find(".cs") == candidateFile.length() - 3) {
			string resultsFile(candidateFile);
			resultsFile.replace(9, 7, "results").replace(resultsFile.find(".cs"), 3, "");

			ts::sl::mockStdout = string();

			(*totalTests)++;
			Tokenizer* tokenizer = new Tokenizer(candidateFile, empty);
			Parser* parser = new Parser(tokenizer, empty);
			ts::Interpreter interpreter(empty);
			interpreter.testing = true;
			interpreter.startInterpretation(ts::Compile(parser, &interpreter));

			totalLines += tokenizer->getTotalLineCount();

			if(isFileEqual(ts::sl::mockStdout, resultsFile)) {
				(*passedTests)++;
			}
			else {
				printError("%s failed test\n", candidateFile.c_str());

				// show diff
				ofstream file;
				file.open(resultsFile + "-diff");
				file << ts::sl::mockStdout;
				file.close();

				system(string("diff " + resultsFile + " " + resultsFile + "-diff").c_str());

				filesystem::remove(resultsFile + "-diff");
			}
		}
	}
	printf("interpreted %d lines\n", totalLines);
}

int runTests(bool overwriteResults) {
	int totalTests = 0;
	int passedTests = 0;
	
	if(!filesystem::is_directory("../tests")) {
		printError("could not find tests");
	}
	
	// regression test some specially crafted files
	for(const auto &entry: filesystem::directory_iterator("../tests/sources/parser")) {
		totalTests++;
		passedTests += parseFileTest("normal code", entry.path().string(), overwriteResults);
	}

	// regression test bl source code
	parseDirectoryTest("../tests/sources/bl-decompiled", overwriteResults, &totalTests, &passedTests);

	// regression test Server_MiniDungeons
	parseDirectoryTest("../tests/sources/Server_MiniDungeons", overwriteResults, &totalTests, &passedTests);

	// regression test Brick_LuaLogic
	parseDirectoryTest("../tests/sources/Brick_LuaLogic", overwriteResults, &totalTests, &passedTests);

	// regression test the interpreter
	interpretDirectoryTest("../tests/sources/interpreter", &totalTests, &passedTests);

	printBoundary();
	printf("passed %d/%d tests\n", passedTests, totalTests);
	printBoundary();

	if(passedTests != totalTests) {
		exit(1);
	}

	return 0;
}
