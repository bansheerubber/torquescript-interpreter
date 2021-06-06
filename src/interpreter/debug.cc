#include "debug.h"

void PrintInstruction(Instruction &instruction) {
	switch(instruction.type) {
		case instruction::INVALID_INSTRUCTION: {
			printf("INVALID_INSTRUCTION;\n");
			break;
		}

		case instruction::NOOP: {
			printf("NOOP;\n");
			break;
		}

		case instruction::PUSH: {
			printf("PUSH {\n");
			printf("   entry type: %d,\n", instruction.push.entry.type);

			if(instruction.push.entry.type == entry::STRING) {
				printf("   entry data: \"%s\",\n", instruction.push.entry.stringData->c_str());
			}
			else {
				printf("   entry data: %f,\n", instruction.push.entry.numberData);
			}

			printf("};\n");
			break;
		}

		case instruction::POP: {
			printf("POP;\n");
			break;
		}

		case instruction::JUMP: {
			printf("JUMP {\n");
			printf("   index: %ld,\n", instruction.jump.index);
			printf("}\n");
			break;
		}

		case instruction::JUMP_IF_TRUE: {
			printf("JUMP_IF_TRUE {\n");
			printf("   index: %ld,\n", instruction.jumpIfTrue.index);
			printf("}\n");
			break;
		}

		case instruction::JUMP_IF_FALSE: {
			printf("JUMP_IF_FALSE {\n");
			printf("   index: %ld,\n", instruction.jumpIfFalse.index);
			printf("}\n");
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
			switch(instruction.type) {
				case instruction::MATH_ADDITION: {
					printf("MATH_ADDITION {\n");
					break;
				}
				case instruction::MATH_SUBTRACT: {
					printf("MATH_SUBTRACT {\n");
					break;
				}
				case instruction::MATH_MULTIPLY: {
					printf("MATH_MULTIPLY {\n");
					break;
				}
				case instruction::MATH_DIVISION: {
					printf("MATH_DIVISION {\n");
					break;
				}
				case instruction::MATH_MODULUS: {
					printf("MATH_MODULUS {\n");
					break;
				}
				case instruction::MATH_SHIFT_LEFT: {
					printf("MATH_SHIFT_LEFT {\n");
					break;
				}

				case instruction::MATH_SHIFT_RIGHT: {
					printf("MATH_SHIFT_RIGHT {\n");
					break;
				}
				
				case instruction::MATH_EQUAL: {
					printf("MATH_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_NOT_EQUAL: {
					printf("MATH_NOT_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_LESS_THAN_EQUAL: {
					printf("MATH_LESS_THAN_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_GREATER_THAN_EQUAL: {
					printf("MATH_GREATER_THAN_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_LESS_THAN: {
					printf("MATH_LESS_THAN {\n");
					break;
				}
				
				case instruction::MATH_GREATER_THAN: {
					printf("MATH_GREATER_THAN {\n");
					break;
				}
				
				case instruction::MATH_BITWISE_AND: {
					printf("MATH_BITWISE_AND {\n");
					break;
				}
				
				case instruction::MATH_BITWISE_OR: {
					printf("MATH_BITWISE_OR {\n");
					break;
				}
				
				case instruction::MATH_BITWISE_XOR: {
					printf("MATH_BITWISE_XOR {\n");
					break;
				}
				
				case instruction::MATH_STRING_EQUAL: {
					printf("MATH_STRING_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_STRING_NOT_EQUAL: {
					printf("MATH_STRING_NOT_EQUAL {\n");
					break;
				}
				
				case instruction::MATH_APPEND: {
					printf("MATH_APPEND {\n");
					break;
				}
				
				case instruction::MATH_SPC: {
					printf("MATH_SPC {\n");
					break;
				}
				
				case instruction::MATH_TAB: {
					printf("MATH_TAB {\n");
					break;
				}
			}

			if(instruction.mathematics.lvalueEntry.type != entry::INVALID) {
				printf("   lvalue type: %d,\n", instruction.mathematics.lvalueEntry.type);
	
				if(instruction.mathematics.lvalueEntry.type == entry::STRING) {
					printf("   lvalue data: \"%s\",\n", instruction.mathematics.lvalueEntry.stringData->c_str());
				}
				else {
					printf("   lvalue data: %f,\n", instruction.mathematics.lvalueEntry.numberData);
				}
			}
			else {
				printf("   lvalue: from stack,\n");
			}

			if(instruction.mathematics.rvalueEntry.type != entry::INVALID) {
				printf("   rvalue type: %d,\n", instruction.mathematics.rvalueEntry.type);
	
				if(instruction.mathematics.rvalueEntry.type == entry::STRING) {
					printf("   rvalue data: \"%s\",\n", instruction.mathematics.rvalueEntry.stringData->c_str());
				}
				else {
					printf("   rvalue data: %f,\n", instruction.mathematics.rvalueEntry.numberData);
				}
			}
			else {
				printf("   rvalue: from stack,\n");
			}

			printf("}\n");

			break;
		}

		case instruction::UNARY_MATHEMATICS: {
			printf("UNARY_MATHEMATICS {\n");
			printf("   operation: %d,\n", instruction.unaryMathematics.operation);
			printf("}\n");
			break;
		}

		case instruction::ARGUMENT_ASSIGN: {
			printf("ARGUMENT_ASSIGN {\n");
			printf("   destination: %s, \n", instruction.argumentAssign.destination.c_str());
			printf("   offset: %d, \n", instruction.argumentAssign.offset);
			printf("   argc: %d, \n", instruction.argumentAssign.argc);
			printf("};\n");
			break;
		}

		case instruction::LOCAL_ASSIGN: {
			printf("LOCAL_ASSIGN {\n");

			printf("   destination: %s, \n", instruction.localAssign.destination.c_str());
			printf("   dimensions: %d, \n", instruction.localAssign.dimensions);
			printf("   push result: %d, \n", instruction.localAssign.pushResult);

			if(instruction.localAssign.fromStack) {
				printf("   from stack: true, \n");
			}
			else {
				printf("   entry type: %d,\n", instruction.localAssign.entry.type);
	
				if(instruction.localAssign.entry.type == entry::STRING) {
					printf("   entry data: \"%s\",\n", instruction.localAssign.entry.stringData->c_str());
				}
				else {
					printf("   entry data: %f,\n", instruction.localAssign.entry.numberData);
				}
			}

			printf("};\n");
			break;
		}

		case instruction::LOCAL_ACCESS: {
			printf("LOCAL_ACCESS {\n");
			printf("   source: %s, \n", instruction.localAccess.source.c_str());
			printf("   dimensions: %d, \n", instruction.localAccess.dimensions);
			printf("};\n");
			
			break;
		}

		case instruction::CALL_FUNCTION: {
			printf("CALL_FUNCTION {\n");
			printf("   name: %s,\n", instruction.callFunction.name.c_str());
			printf("   cachedIndex: %ld,\n", instruction.callFunction.cachedIndex);
			printf("   isCached: %d,\n", instruction.callFunction.isCached);
			printf("};\n");
			break;
		}

		case instruction::RETURN: {
			printf("RETURN;\n");
			break;
		}

		case instruction::POP_ARGUMENTS: {
			printf("POP_ARGUMENTS;\n");
			break;
		}
	}
}