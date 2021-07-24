from gen.gen import get_generated_code
import sys

entry_name = sys.argv[1]
output_name = sys.argv[2]
source_type = sys.argv[3]
output_type = sys.argv[4]

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

if "_" in source_type:
	argument_types = source_type.split("_")
	
	conditional = "if"
	for entry_type in argument_types:
		code = entry_type_conversion[entry_type][output_type]
		code = code\
			.replace("%%conditional%%", conditional)\
			.replace("%%entry%%", entry_name)\
			.replace("%%output%%", output_name)
		
		print(code)
		
		conditional = "else if"
else:
	code = entry_type_conversion[source_type][output_type]
	code = code\
		.replace("%%conditional%%", "if")\
		.replace("%%entry%%", entry_name)\
		.replace("%%output%%", output_name)
	
	print(code)