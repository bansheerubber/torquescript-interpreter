#pragma once

#define TS_INTERPRETER_PREFIX true

// here lies some pretty nasty macros
#define TS_MATH_DEFINE_NUMBER_VALUES_PREFIX()\
	Entry* lvalue;\
	Entry* rvalue;\
	/* we have to define lvalue first when working with prefix */\
	if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {\
		lvalue = &instruction.mathematics.lvalueEntry;\
	}\
	else {\
		lvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {\
		rvalue = &instruction.mathematics.rvalueEntry;\
	}\
	else {\
		rvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	double lvalueNumber = 0;\
	double rvalueNumber = 0;\
	if(lvalue->type == entry::NUMBER) {/* typecheck lvalue */\
		lvalueNumber = lvalue->numberData;\
	}\
	else {\
		lvalueNumber = stringToNumber(*lvalue->stringData);\
	}\
	if(rvalue->type == entry::NUMBER) {/* typecheck rvalue */\
		rvalueNumber = rvalue->numberData;\
	}\
	else {\
		rvalueNumber = stringToNumber(*rvalue->stringData);\
	}

#define TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX()\
	Entry* lvalue;\
	Entry* rvalue;\
	/* we have to define rvalue first when working with prefix */\
	if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {\
		rvalue = &instruction.mathematics.rvalueEntry;\
	}\
	else {\
		rvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {\
		lvalue = &instruction.mathematics.lvalueEntry;\
	}\
	else {\
		lvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	double lvalueNumber = 0;\
	double rvalueNumber = 0;\
	if(lvalue->type == entry::NUMBER) {/* typecheck lvalue */\
		lvalueNumber = lvalue->numberData;\
	}\
	else {\
		lvalueNumber = stringToNumber(*lvalue->stringData);\
	}\
	if(rvalue->type == entry::NUMBER) {/* typecheck rvalue */\
		rvalueNumber = rvalue->numberData;\
	}\
	else {\
		rvalueNumber = stringToNumber(*rvalue->stringData);\
	}

#define TS_MATH_DEFINE_STRING_VALUES_PREFIX()\
	Entry* lvalue;\
	Entry* rvalue;\
	/* we have to define lvalue first when working with prefix */\
	if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {\
		lvalue = &instruction.mathematics.lvalueEntry;\
	}\
	else {\
		lvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {\
		rvalue = &instruction.mathematics.rvalueEntry;\
	}\
	else {\
		rvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	string* lvalueString = nullptr;\
	string* rvalueString = nullptr;\
	bool deleteLValueString = false;/* if we need to delete the string if we converted from number to string */\
	bool deleteRValueString = false;\
	if(lvalue->type == entry::STRING) {/* typecheck lvalue */\
		lvalueString = lvalue->stringData;\
	}\
	else {\
		lvalueString = numberToString(lvalue->numberData);\
		deleteLValueString = true;\
	}\
	if(rvalue->type == entry::STRING) {/* typecheck rvalue */\
		rvalueString = rvalue->stringData;\
	}\
	else {\
		rvalueString = numberToString(rvalue->numberData);\
		deleteRValueString = true;\
	}

#define TS_MATH_DEFINE_STRING_VALUES_POSTFIX()\
	Entry* lvalue;\
	Entry* rvalue;\
	/* we have to define rvalue first when working with prefix */\
	if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {\
		rvalue = &instruction.mathematics.rvalueEntry;\
	}\
	else {\
		rvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {\
		lvalue = &instruction.mathematics.lvalueEntry;\
	}\
	else {\
		lvalue = &this->stack[this->stackPointer - 1];\
		this->pop();\
	}\
	string* lvalueString = nullptr;\
	string* rvalueString = nullptr;\
	bool deleteLValueString = false;/* if we need to delete the string if we converted from number to string */\
	bool deleteRValueString = false;\
	if(lvalue->type == entry::STRING) {/* typecheck lvalue */\
		lvalueString = lvalue->stringData;\
	}\
	else {\
		lvalueString = numberToString(lvalue->numberData);\
		deleteLValueString = true;\
	}\
	if(rvalue->type == entry::STRING) {/* typecheck rvalue */\
		rvalueString = rvalue->stringData;\
	}\
	else {\
		rvalueString = numberToString(rvalue->numberData);\
		deleteRValueString = true;\
	}
