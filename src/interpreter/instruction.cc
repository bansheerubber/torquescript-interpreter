#include "instruction.h"

void ts::copyInstruction(Instruction &source, Instruction &destination) {
	destination.type = source.type;
	destination.index = source.index;
	switch(source.type) {
		case instruction::PUSH: {
			copyEntry(source.push.entry, destination.push.entry);
			break;
		}

		case instruction::JUMP: {
			destination.jump.instruction = source.jump.instruction;
			break;
		}

		case instruction::JUMP_IF_TRUE: {
			destination.jumpIfTrue.instruction = source.jumpIfTrue.instruction;
			destination.jumpIfTrue.pop = source.jumpIfTrue.pop;
			break;
		}

		case instruction::JUMP_IF_FALSE: {
			destination.jumpIfFalse.instruction = source.jumpIfFalse.instruction;
			destination.jumpIfFalse.pop = source.jumpIfFalse.pop;
			break;
		}

		case instruction::MATH_ADDITION:
		case instruction::MATH_SUBTRACT:
		case instruction::MATH_MULTIPLY:
		case instruction::MATH_DIVISION:
		case instruction::MATH_MODULUS:
		case instruction::MATH_SHIFT_LEFT:
		case instruction::MATH_SHIFT_RIGHT:
		case instruction::MATH_EQUAL:
		case instruction::MATH_NOT_EQUAL:
		case instruction::MATH_LESS_THAN_EQUAL:
		case instruction::MATH_GREATER_THAN_EQUAL:
		case instruction::MATH_LESS_THAN:
		case instruction::MATH_GREATER_THAN:
		case instruction::MATH_BITWISE_AND:
		case instruction::MATH_BITWISE_OR:
		case instruction::MATH_BITWISE_XOR:
		case instruction::MATH_STRING_EQUAL:
		case instruction::MATH_STRING_NOT_EQUAL:
		case instruction::MATH_APPEND:
		case instruction::MATH_SPC:
		case instruction::MATH_TAB: {
			copyEntry(source.mathematics.lvalueEntry, destination.mathematics.lvalueEntry);
			copyEntry(source.mathematics.rvalueEntry, destination.mathematics.rvalueEntry);
			break;
		}

		case instruction::UNARY_MATHEMATICS: {
			destination.unaryMathematics.operation = source.unaryMathematics.operation;
			break;
		}

		case instruction::ARGUMENT_ASSIGN: {
			destination.argumentAssign.offset = source.argumentAssign.offset;
			destination.argumentAssign.argc = source.argumentAssign.argc;
			new((void*)&destination.argumentAssign.destination) string(source.argumentAssign.destination); // TODO move this initialization elsewhere
			destination.argumentAssign.hash = source.argumentAssign.hash;
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			destination.localAssign.dimensions = source.localAssign.dimensions;
			destination.localAssign.pushResult = source.localAssign.pushResult;
			destination.localAssign.fromStack = source.localAssign.fromStack;
			if(!destination.localAssign.fromStack) {
				copyEntry(source.localAssign.entry, destination.localAssign.entry);
			}

			new((void*)&destination.localAssign.destination) string(source.localAssign.destination); // TODO move this initialization elsewhere
			destination.localAssign.hash = source.localAssign.hash;
			destination.localAssign.operation = source.localAssign.operation;
			break;
		}

		case instruction::LOCAL_ACCESS: {
			destination.localAccess.dimensions = source.localAccess.dimensions;
			new((void*)&destination.localAccess.source) string(source.localAccess.source); // TODO move this initialization elsewhere
			destination.localAccess.hash = source.localAccess.hash;
			break;
		}

		case instruction::CALL_FUNCTION: {
			new((void*)&destination.callFunction.name) string(source.callFunction.name); // TODO move this initialization elsewhere
			destination.callFunction.cachedIndex = source.callFunction.cachedIndex;
			destination.callFunction.isCached = source.callFunction.isCached;
			destination.callFunction.isTSSL = source.callFunction.isTSSL;
			break;
		}

		default: {
			break;
		}
	}
}