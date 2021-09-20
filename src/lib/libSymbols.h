#pragma once

#include <stdarg.h>

extern "C" {	
	typedef void* tsEnginePtr;
	#define tsPrintFunction(name)		int (*name)(const char* format, ...)
	#define tsVPrintFunction(name)		int (*name)(const char* format, va_list args)

	enum tsEntryType {
		TS_ENTRY_INVALID,
		TS_ENTRY_NUMBER,
		TS_ENTRY_STRING,
		TS_ENTRY_OBJECT,
	};
	
	typedef struct tsObjectWrapper {
		void* object;
		void* data;
	} tsObjectWrapper;
	typedef tsObjectWrapper* tsObjectWrapperPtr;

	typedef struct tsObjectReference {
		tsObjectWrapperPtr objectWrapper;
		unsigned long id = 0;
		void* __pad1;
		void* __pad2;
	} tsObjectReference;
	typedef tsObjectReference* tsObjectReferencePtr;

	typedef struct tsEntry {
		tsEntryType type;
		union {
			double numberData;
			char* stringData;
			tsObjectReferencePtr objectData;
		};
	} tsEntry;
	typedef tsEntry* tsEntryPtr;
	
	typedef tsEntryPtr (*tsFunctionPtr)(tsEnginePtr engine, unsigned int argc, tsEntryPtr args);
	
	tsEnginePtr tsCreateEngine(char isParallel);

	bool tsTick(tsEnginePtr engine);
	void tsSetTickRate(tsEnginePtr engine, long tickRate);
	void tsExecFile(tsEnginePtr engine, const char* filename);
	void tsEval(tsEnginePtr engine, const char* shell);
	void tsSetPrintFunction(tsPrintFunction(print), tsPrintFunction(warning), tsPrintFunction(error));
	void tsSetVPrintFunction(tsVPrintFunction(print), tsVPrintFunction(warning), tsVPrintFunction(error));
	void tsRegisterNamespace(tsEnginePtr engine, const char* nameSpace);
	void tsNamespaceInherit(tsEnginePtr engine, const char* parent, const char* child);
	tsObjectReferencePtr tsCreateObject(tsEnginePtr engine, const char* nameSpace, void* data);
	void tsDeleteObject(tsObjectReferencePtr objectReference);
	const char* tsGetNamespaceFromObject(tsObjectReferencePtr object);
	int tsCompareNamespaceToObject(tsObjectReferencePtr object, const char* nameSpace);
	void tsRegisterFunction(tsEnginePtr engine, tsEntryType returnType, tsFunctionPtr function, const char* name, unsigned int argCount, tsEntryType* argTypes);
	void tsRegisterMethod(tsEnginePtr engine, tsEntryType returnType, tsFunctionPtr function, const char* nameSpace, const char* name, unsigned int argCount, tsEntryType* argTypes);
}
