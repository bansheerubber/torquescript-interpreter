from gen.gen import get_generated_code
import sys

entry_name = sys.argv[1]
output_name = sys.argv[2]
source_type = sys.argv[3]
output_type = sys.argv[4]
additional = sys.argv[5] if len(sys.argv) >= 6 else ""
interpreter = sys.argv[6] if len(sys.argv) >= 7 else "this"

entry_types = [
	"NUMBER",
	"STRING",
	"OBJECT",
]

entry_type_conversion = {}

for entry_type1 in entry_types:
	for entry_type2 in entry_types:
		try:
			code = get_generated_code(
				"types",
				entry_type1.lower() + "To" + entry_type2.lower().title(),
				0
			)

			if entry_type1 not in entry_type_conversion:
				entry_type_conversion[entry_type1] = {}

			entry_type_conversion[entry_type1][entry_type2] = code
		except:
			pass

def get_delete_line(variable_name, entry_type):
	if variable_name == None or variable_name == "":
		return ""
	
	value = "true" if output_type != "STRING" else "false"
	return f"{variable_name} = {value};"

if "_" in source_type:
	argument_types = source_type.split("_")
	
	conditional = "if"
	for entry_type in argument_types:
		code = entry_type_conversion[entry_type][output_type]
		code = code\
			.replace("%%conditional%%", conditional)\
			.replace("%%entry%%", entry_name)\
			.replace("%%output%%", output_name)\
			.replace("%%delete%%", get_delete_line(additional, entry_type))\
			.replace("%%this%%", interpreter)
		
		print(code)
		
		conditional = "else if"
else:
	code = entry_type_conversion[source_type][output_type]
	code = code\
		.replace("%%conditional%%", "if")\
		.replace("%%entry%%", entry_name)\
		.replace("%%output%%", output_name)\
		.replace("%%delete%%", get_delete_line(additional, output_type))\
		.replace("%%this%%", interpreter)
	
	print(code)