#include <chrono>
#include <filesystem>
#include <future>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>

#ifdef __linux__
#include <unistd.h>
#endif

#include "args.h"
#include "io.h"
#include "./tokenizer/tokenizer.h"
#include "./parser/parser.h"

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

void parseThread(vector<string> paths, ParsedArguments args, promise<int> &&p) {
	int total = 0;
	for(string path: paths) {
		Tokenizer* tokenizer = new Tokenizer(path, args);
		Parser* parser = new Parser(tokenizer, args);
		total += tokenizer->getTotalLineCount();
	}
	p.set_value(total);
}

int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false);
	
	vector<Argument> arguments = createArguments();

	bool isPiped = isPipe();

	// parse arguments
	ParsedArguments args = parseArguments(arguments, argc, argv);
	if(args.arguments["help"] != "") {
		printHelp(arguments);
		return 0;
	}

	if(args.argumentError) {
		printError("error: expected correct arguments\n\n");
		printHelp(arguments);
		return 1;
	}

	if(args.files.size() == 0 && !isPiped) {
		printError("error: expected files/directories\n\n");
		printHelp(arguments);
		return 1;
	}

	unsigned int maxThreadCount = thread::hardware_concurrency();
	if(args.arguments["threads"] != "") {
		try {
			maxThreadCount = stoi(args.arguments["threads"]);
			if(maxThreadCount == 0) {
				maxThreadCount = 1;
			}
		}
		catch(...) {
			printError("error: expected integer for max thread count\n\n");
			printHelp(arguments);
			return 1;
		}
	}
	
	if(isPiped) {
		string file;
		string line;
		while(getline(cin, line)) {
			file += line + "\n";
		}

		args.arguments["piped"] = "true"; // tell parser that input was piped

		Tokenizer* tokenizer = new Tokenizer(file, true, args);
		Parser* parser = new Parser(tokenizer, args);
	}
	else {
		for(string fileName: args.files) {
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
					threads.push_back(thread(parseThread, pathsForThread, args, move(p)));
				}

				int totalLines = 0;
				for(int i = 0; i < (int)threads.size(); i++) {
					thread &t = threads[i];
					t.join();

					future<int> &f = futures[i];
					totalLines += f.get();
				}

				float time = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() / 1000.0;
				cout << "completed parsing " << totalLines << " lines from " << path.string() << " in " << fixed << setprecision(2) << time << "s" << endl;
			}
			else if(filesystem::is_regular_file(path, error)) {
				auto start = chrono::high_resolution_clock::now();
				
				Tokenizer* tokenizer = new Tokenizer(fileName, args);
				Parser* parser = new Parser(tokenizer, args);

				float time = (float)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() / 1000.0;
				cout << "completed parsing " << tokenizer->getTotalLineCount() << " lines from " << path.string() << " in " << fixed << setprecision(2) << time << "s" << endl;
			}
			else {
				printError("error opening file or directory %s\n", fileName.c_str());
			}
		}
	}
	
	return 0;
}