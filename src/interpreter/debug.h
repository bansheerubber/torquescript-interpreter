#pragma once

#include "instruction.h"

using namespace ts;

void PrintInstruction(Instruction &instruction);
const char* InstructionTypeToName(instruction::InstructionType type);
