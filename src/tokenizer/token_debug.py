import re

file = open("token.h", "r")

print("static const char* TokenTypeDebug[] = {")

read = False
for line in file:
	if read:
		if match := re.match(r"^[\s]*([A-Za-z0-9_]+)", line):
			print(f'\t"{match.group(1)}",')
		else:
			read = False
	
	if "enum TokenType" in line:
		read = True

print("};")