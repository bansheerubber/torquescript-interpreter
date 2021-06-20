#include "debug.h"
#include "object.h"

void PrintInstruction(Instruction &instruction) {
	switch(instruction.type) {
		## instruction_generator.py debug.cc
	}
}

## instruction_debug.py debug.cc

const char* InstructionTypeToName(instruction::InstructionType type) {
	return InstructionTypeDebug[type];
}