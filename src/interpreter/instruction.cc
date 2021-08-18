#include "instruction.h"

#include "../util/allocateString.h"

void ts::copyInstruction(Instruction &source, Instruction &destination) {
	destination.type = source.type;
	destination.pushType = source.pushType;
	destination.index = source.index;
	switch(source.type) {
		## instruction_generator.py instruction.cc

		default: {
			break;
		}
	}
}