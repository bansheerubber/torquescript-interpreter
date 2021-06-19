from gen.gen import get_generated_code

number_operations = {
	"MATH_ADDITION": "this->push({0} + {1});",
	"MATH_SUBTRACT": "this->push({0} - {1});",
	"MATH_MULTIPLY": "this->push({0} * {1});",
	"MATH_DIVISION": "this->push({0} / {1});",
	"MATH_MODULUS": "this->push((int){0} % (int){1});",
	"MATH_SHIFT_LEFT": "this->push((int){0} << (int){1});",
	"MATH_SHIFT_RIGHT": "this->push((int){0} >> (int){1});",
	"MATH_EQUAL": "this->push({0} == {1});",
	"MATH_NOT_EQUAL": "this->push({0} != {1});",
	"MATH_LESS_THAN_EQUAL": "this->push({0} <= {1});",
	"MATH_GREATER_THAN_EQUAL": "this->push({0} >= {1});",
	"MATH_LESS_THAN": "this->push({0} < {1});",
	"MATH_GREATER_THAN": "this->push({0} > {1});",
	"MATH_BITWISE_AND": "this->push((int){0} & (int){1});",
	"MATH_BITWISE_OR": "this->push((int){0} | (int){1});",
	"MATH_BITWISE_XOR": "this->push((int){0} ^ (int){1});",
}

string_operations = {
	"MATH_STRING_EQUAL": "this->push(toLower(*{0}).compare(toLower(*{1})) == 0);",
	"MATH_STRING_NOT_EQUAL": "this->push(toLower(*{0}).compare(toLower(*{1})) != 0);",
	"MATH_APPEND": """string* stringResult = new string(*{0});
			*stringResult += *{1};
			this->push(stringResult);""",
	"MATH_SPC": """string* stringResult = new string(*{0});
			*stringResult += ' ' + *{1};
			this->push(stringResult);""",
	"MATH_TAB": """string* stringResult = new string(*{0});
			*stringResult += '\\t' + *{1};
			this->push(stringResult);""",
	"MATH_NL": """string* stringResult = new string(*{0});
			*stringResult += '\\n' + *{1};
			this->push(stringResult);""",
}

# handle number instructions
for instruction, operation in number_operations.items():
	formatted = operation.format("lvalueNumber", "rvalueNumber")
	NUMBER_MATH_MACRO = get_generated_code("math", "numbers", 3)

	print(f"""		case instruction::{instruction}: {{
{NUMBER_MATH_MACRO}

			{formatted}
			break;
		}}\n""")

# handle string instructions
for instruction, operation in string_operations.items():
	formatted = operation.format("lvalueString", "rvalueString")
	STRING_MATH_MACRO = get_generated_code("math", "strings", 3)
	DELETE_STRINGS_MACRO = get_generated_code("math", "deleteStrings", 3)

	print(f"""		case instruction::{instruction}: {{
{STRING_MATH_MACRO}

			{formatted}
{DELETE_STRINGS_MACRO}
			break;
		}}\n""")