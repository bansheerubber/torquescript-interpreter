import sys
sys.path.insert(0, "../../tools")
from gen import get_generated_code

arguments = {
	"first": ["%%comparison%%", "%%words%%"],
	"getCount": ["%%comparison%%", "%%words%%"],
	"getMultiple": ["%%comparison%%", "%%words%%", "%%start%%", "%%end%%"],
	"getSingle": ["%%comparison%%", "%%words%%", "%%index%%"],
	"remove": ["%%comparison%%", "%%words%%", "%%index%%"],
	"rest": ["%%comparison%%", "%%words%%"],
	"set": ["%%comparison%%", "%%words%%", "%%index%%", "%%replacement%%"],
}

kind = sys.argv[1]
generated = get_generated_code("tokenizing", kind, 0)
for i in range(2, len(sys.argv)):
	replacement = arguments[kind][i - 2]
	if replacement == "%%comparison%%":
		comparison = []
		buffer = ""
		for character in sys.argv[i]:
			if character == "\\":
				buffer = character
				continue
			
			buffer = buffer + character
			comparison.append("character == '" + buffer + "'")
			buffer = ""
		generated = generated.replace(replacement, " || ".join(comparison))
	else:
		generated = generated.replace(replacement, sys.argv[i])
print(generated)
