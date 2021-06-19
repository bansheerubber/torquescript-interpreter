# handles various assignment instructions

from assignment_instructions import get_suffixes, get_prefixes
from gen.gen import get_generated_code

operations = {
	"INCREMENT": "++{0}",
	"DECREMENT": "--{0}",
	"PLUS": "{0} + {1}",
	"MINUS": "{0} - {1}",
	"ASTERISK": "{0} * {1}",
	"SLASH": "{0} / {1}",
	"MODULUS": "(int){0} % (int){1}",
	"SHIFT_LEFT": "(int){0} << (int){1}",
	"SHIFT_RIGHT": "(int){0} >> (int){1}",
	"BITWISE_AND": "(int){0} & (int){1}",
	"BITWISE_XOR": "(int){0} ^ (int){1}",
	"BITWISE_OR": "(int){0} | (int){1}",
}

no_get = ["INCREMENT", "DECREMENT"]

# generates instruction handlers in switch statement
for prefix, folder in get_prefixes().items():
	START_MACRO = get_generated_code(folder, "start", 3)
	GET_VALUE_MACRO = get_generated_code(folder, "getValue", 3)
	CONVERT_MACRO = get_generated_code(folder, "convert", 3)
	END_MACRO = get_generated_code(folder, "end", 3)

	for suffix in get_suffixes():
		if suffix != "EQUAL": # handle all other instructions using the files we pulled from
			operation = operations[suffix].format("entryNumber", "valueNumber")
			formatted = GET_VALUE_MACRO.replace(r"%%", "value")
			get_macro = f"{formatted}\n{CONVERT_MACRO}" if suffix not in no_get else ""
			
			print(f"""		case instruction::{prefix}_{suffix}: {{
{START_MACRO}{get_macro}
			entry->setNumber({operation});\n
{END_MACRO}
			break;
		}}\n""")
		else: # handle equals as a special case
			formatted = GET_VALUE_MACRO.replace(r"%%", "entry")
			print(f"""		case instruction::{prefix}_{suffix}: {{
{formatted}
			this->topContext->setVariableEntry(
				instruction,
				instruction.localAssign.destination,
				instruction.localAssign.hash,
				*entry
			);\n
{END_MACRO}
			break;
		}}\n""")