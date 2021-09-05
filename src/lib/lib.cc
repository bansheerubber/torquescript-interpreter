#include "libSymbols.h"

#include <stdio.h>
#include <string>

#include "../args.h"
#include "../compiler/compiler.h"
#include "../engine/engine.h"
#include "../interpreter/interpreter.h"
#include "../parser/parser.h"
#include "../tokenizer/tokenizer.h"

using namespace std;

tsEngine tsCreateEngine(char isParallel) {
	ParsedArguments args = {};
	return new Engine(args, isParallel);
}

bool tsTick(tsEngine engine) {
	// only tick if we're not in parallel mode
	if(!((ts::Engine*)engine)->interpreter->isParallel) {
		return ((ts::Engine*)engine)->interpreter->tick();
	}
	return false;
}

void tsSetTickRate(tsEngine engine, long tickRate) {
	((ts::Engine*)engine)->interpreter->setTickRate(tickRate);
}

void tsExecFile(tsEngine engine, const char* filename) {
	((ts::Engine*)engine)->execFile(string(filename));
}