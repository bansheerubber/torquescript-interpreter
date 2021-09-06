#pragma once

#include <stdarg.h>

extern "C" {	
	typedef void* tsEngine;
	#define tsPrintFunction(name)		int (*name)(const char* format, ...)
	#define tsVPrintFunction(name)		int (*name)(const char* format, va_list args)

	typedef struct tsObjectWrapper {
		void* object;
		void* customData;
	} tsObjectWrapper;
	
	tsEngine tsCreateEngine(char isParallel);

	bool tsTick(tsEngine engine);
	void tsSetTickRate(tsEngine engine, long tickRate);
	void tsExecFile(tsEngine engine, const char* filename);
	void tsSetPrintFunction(tsPrintFunction(print), tsPrintFunction(warning), tsPrintFunction(error));
	void tsSetVPrintFunction(tsVPrintFunction(print), tsVPrintFunction(warning), tsVPrintFunction(error));
}
