# includable file for getting assignment instructions

suffixes = [
	"EQUAL",
	"INCREMENT",
	"DECREMENT",
	"PLUS",
	"MINUS",
	"ASTERISK",
	"SLASH",
	"MODULUS",
	"SHIFT_LEFT",
	"SHIFT_RIGHT",
	"BITWISE_AND",
	"BITWISE_XOR",
	"BITWISE_OR",
]

prefixes = {
	"LOCAL_ASSIGN": "localAssign"
}

# get instructions for just the prefix
def get_instructions(prefix):
	output = []
	for suffix in suffixes:
		output.append(f"{prefix}_{suffix}")
	return output

# get all instructions
def get_all_instructions():
	output = []
	for prefix in prefixes.keys():
		for suffix in suffixes:
			output.append(f"{prefix}_{suffix}")
	return output

def get_suffixes():
	return suffixes

def get_prefixes():
	return prefixes