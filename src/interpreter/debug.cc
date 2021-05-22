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

		case instruction::MATHEMATICS: {
			printf("MATHEMATICS {\n");
			printf("   operator type: %d,\n", instruction.mathematics.operation);

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
				printf("   lvalue stack: %d,\n", instruction.mathematics.lvalue);
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
				printf("   rvalue stack: %d,\n", instruction.mathematics.rvalue);
			}
			
			printf("};\n");
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

		case instruction::NEW_FRAME: {
			printf("NEW_FRAME;\n");
			break;
		}

		case instruction::DELETE_FRAME: {
			printf("DELETE_FRAME {\n");
			printf("   save: %d,\n", instruction.deleteFrame.save);
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
	}
}