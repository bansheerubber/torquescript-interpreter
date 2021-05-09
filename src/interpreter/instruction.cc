#include "instruction.h"

void ts::copyInstruction(Instruction &source, Instruction &destination) {
	destination.type = source.type;
	switch(source.type) {
		case instruction::PUSH: {
			copyEntry(source.push.entry, destination.push.entry);
			break;
		}

		case instruction::JUMP: {
			destination.jump.jumpPoint = source.jump.jumpPoint;
			break;
		}

		case instruction::MATHEMATICS: {
			destination.mathematics.lvalue = source.mathematics.lvalue;
			destination.mathematics.rvalue = source.mathematics.rvalue;

			copyEntry(source.mathematics.lvalueEntry, destination.mathematics.lvalueEntry);
			copyEntry(source.mathematics.rvalueEntry, destination.mathematics.rvalueEntry);

			destination.mathematics.operation = source.mathematics.operation;
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			destination.localAssign.destination = source.localAssign.destination;
			destination.localAssign.fromStack = source.localAssign.fromStack;
			copyEntry(source.localAssign.entry, destination.localAssign.entry);
			destination.localAssign.dimensions = source.localAssign.dimensions;
			break;
		}

		case instruction::LOCAL_ACCESS: {
			destination.localAccess.source = source.localAccess.source;
			destination.localAccess.dimensions = source.localAccess.dimensions;
			break;
		}

		case instruction::DELETE_FRAME: {
			destination.deleteFrame.save = source.deleteFrame.save;
			break;
		}

		default: {
			break;
		}
	}
}