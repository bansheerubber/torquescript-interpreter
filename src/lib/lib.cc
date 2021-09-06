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

void tsSetPrintFunction(tsEngine engine, tsPrintFunction(print), tsPrintFunction(warning), tsPrintFunction(error)) {
	((ts::Engine*)engine)->printFunction = print;
	((ts::Engine*)engine)->warningFunction = warning;
	((ts::Engine*)engine)->errorFunction = error;
}

void tsVSetPrintFunction(tsEngine engine, tsVPrintFunction(print), tsVPrintFunction(warning), tsVPrintFunction(error)) {
	((ts::Engine*)engine)->vPrintFunction = print;
	((ts::Engine*)engine)->vWarningFunction = warning;
	((ts::Engine*)engine)->vErrorFunction = error;
}
