import re
import sys

from assignment_instructions import get_assignment_instructions
from math_instructions import get_math_instructions

structs = {
	"push": ["PUSH"],
	"jump": ["JUMP"],
	"jumpIfTrue": ["JUMP_IF_TRUE"],
	"jumpIfFalse": ["JUMP_IF_FALSE"],
	"mathematics": get_math_instructions(),
	"unaryMathematics": ["UNARY_MATHEMATICS"],
	"localAssign": get_assignment_instructions("LOCAL_ASSIGN"),
	"globalAssign": get_assignment_instructions("GLOBAL_ASSIGN"),
	"localAccess": ["LOCAL_ACCESS"],
	"globalAccess": ["GLOBAL_ACCESS"],
	"createObject": ["CREATE_OBJECT"],
	"callFunction": ["CALL_FUNCTION"],
	"callObject": ["CALL_OBJECT"],
	"objectAssign": get_assignment_instructions("OBJECT_ASSIGN"),
	"objectAccess": ["OBJECT_ACCESS"],
	"popArguments": ["POP_ARGUMENTS"],
	"linkVariable": ["LINK_VARIABLE"],
}

instruction_to_struct = {instruction: struct for struct in structs.keys() for instruction in structs[struct]}

all_instructions = []

file = open("instruction.h", "r")
flag = 0
brace_count = -1
buffer = []
struct_to_types = {}
loading_instructions = False
for line in file:
	if loading_instructions:
		if "};" in line:
			loading_instructions = False
		else:
			all_instructions.append(re.match(r'(\w+),', line.strip()).group(1))
	elif "enum InstructionType {" in line:
		loading_instructions = True
	
	if flag == 2:
		if "{" in line:
			brace_count = brace_count + 1
		elif "}" in line:
			brace_count = brace_count - 1
		
		if match := re.match(r'(^[\w\s*:_]+?);', line.strip()):
			split = match.group(1).split(' ')
			variable_type = ' '.join(split[:-1])
			variable_name = split[-1]

			if variable_name[0] == "*":
				variable_type = variable_type + "*"
				variable_name = variable_name[1:]
			
			buffer.append((variable_type, variable_name))
		
		if brace_count == 1 and line.strip() != '':
			struct_name = re.match(r'\}[\s]*([\w]+)', line.strip()).group(1)
			struct_to_types[struct_name] = buffer
			buffer = []
	
	if "struct Instruction {" in line:
		flag = 1
	elif flag == 1 and "union {" in line:
		flag = 2
		brace_count = 1
	
	if brace_count == 0:
		break

if sys.argv[1] == "instruction.cc":
	for key, values in structs.items():
		newline = ''
		for value in values:
			print(f"{newline}case instruction::{value}:", end='')
			newline = '\n'
		
		print(" {")

		for variable_type, variable_name in struct_to_types[key]:
			if variable_type == "string":
				print(f"	new((void*)&destination.{key}.{variable_name}) string(source.{key}.{variable_name});")
			elif variable_type == "Entry":
				print(f"	destination.{key}.{variable_name} = ts::Entry();")
				print(f"	copyEntry(source.{key}.{variable_name}, destination.{key}.{variable_name});")
			else:
				print(f"	destination.{key}.{variable_name} = source.{key}.{variable_name};")
		
		print("	break;")
		print("}\n")
elif sys.argv[1] == "debug.cc":
	for instruction in all_instructions:
		print(f"case instruction::{instruction}: {{")

		if instruction not in instruction_to_struct:
			print(f'	printf("{instruction};\\n");')
			print("	break;")
			print("}\n")
			continue
		
		print(f'	printf("{instruction} {{\\n");')
		struct = instruction_to_struct[instruction]
		for variable_type, variable_name in struct_to_types[struct]:
			if "Instruction*" in variable_type: # handle things we don't want to print
				pass
			elif variable_type == "bool": # handle booleans
				print(f"""	if(instruction.{struct}.{variable_name}) {{
		printf("   {variable_name}: true;\\n");
	}}
	else {{
		printf("   {variable_name}: false;\\n");
	}}""")
			elif variable_type == "string": # handle strings
				print(f'	printf("   {variable_name}: %s,\\n", instruction.{struct}.{variable_name}.c_str());')
			elif "long" in variable_type or "size_t" in variable_type: # handle longs
				print(f'	printf("   {variable_name}: %ld,\\n", instruction.{struct}.{variable_name});')
			elif variable_type == "Entry": # handle entries
				print(f"""	if(instruction.{struct}.{variable_name}.type != entry::INVALID) {{
		printf("   {variable_name} type: %d,\\n", instruction.{struct}.{variable_name}.type);

		if(instruction.{struct}.{variable_name}.type == entry::STRING) {{
			printf("   {variable_name} data: \\"%s\\",\\n", instruction.{struct}.{variable_name}.stringData);
		}}
		else if(instruction.{struct}.{variable_name}.type == entry::OBJECT) {{
			printf("   {variable_name} data: %ld,\\n", instruction.{struct}.{variable_name}.objectData->object->id);
		}}
		else {{
			printf("   {variable_name} data: %f,\\n", instruction.{struct}.{variable_name}.numberData);
		}}
	}}
	else {{
		printf("   {variable_name}: invalid entry,\\n");
	}}""")
			elif variable_type == "size_t" or variable_type == "relative_stack_location" or variable_type == "stack_location": # handle size_t
				print(f'	printf("   {variable_name}: %ld,\\n", instruction.{struct}.{variable_name});')
			else: # handle everything else
				print(f'	printf("   {variable_name}: %d,\\n", instruction.{struct}.{variable_name});')
		
		print(f'	printf("}};\\n");')
		print("	break;")
		print("}\n")

file.close()