import re

file = open("instruction.h", "r")

print("static const char* InstructionTypeDebug[] = {")

read = False
for line in file:
	if read:
		if match := re.match(r"^[\s]*([A-Za-z0-9_]+)", line):
			print(f'\t"{match.group(1)}",')
		else:
			read = False
	
	if "enum InstructionType" in line:
		read = True

print("};")