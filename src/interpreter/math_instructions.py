instructions = {
	"MATH_ADDITION": {
		"type": 0,
		"operator": "+",
	},
	"MATH_SUBTRACT": {
		"type": 0,
		"operator": "-",
	},
	"MATH_MULTIPLY": {
		"type": 0,
		"operator": "*",
	},
	"MATH_DIVISION": {
		"type": 0,
		"operator": "/",
	},
	"MATH_MODULUS": {
		"type": 1,
		"operator": "%",
	},
	"MATH_SHIFT_LEFT": {
		"type": 1,
		"operator": "<<",
	},
	"MATH_SHIFT_RIGHT": {
		"type": 1,
		"operator": ">>",
	},
	"MATH_EQUAL": {
		"type": 0,
		"operator": "==",
	},
	"MATH_NOT_EQUAL": {
		"type": 0,
		"operator": "!=",
	},
	"MATH_LESS_THAN_EQUAL": {
		"type": 0,
		"operator": "<=",
	},
	"MATH_GREATER_THAN_EQUAL": {
		"type": 0,
		"operator": ">=",
	},
	"MATH_LESS_THAN": {
		"type": 0,
		"operator": "<",
	},
	"MATH_GREATER_THAN": {
		"type": 0,
		"operator": ">",
	},
	"MATH_BITWISE_AND": {
		"type": 1,
		"operator": "&",
	},
	"MATH_BITWISE_OR": {
		"type": 1,
		"operator": "|",
	},
	"MATH_BITWISE_XOR": {
		"type": 1,
		"operator": "^",
	},
}

for key, value in instructions.items():
	cast = ""
	if value["type"] == 1:
		cast = "(int)"

	print(f"""case instruction::{key}: {{
		#if TS_INTERPRETER_PREFIX
			TS_MATH_DEFINE_NUMBER_VALUES_PREFIX();
		#else
			TS_MATH_DEFINE_NUMBER_VALUES_POSTFIX();
		#endif
		
		this->push({cast}lvalueNumber {value["operator"]} {cast}rvalueNumber);
		break;
	}}\n""")