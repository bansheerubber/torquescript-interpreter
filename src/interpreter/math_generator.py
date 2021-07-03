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
	"MATH_STRING_EQUAL": "this->push(stringCompareInsensitive({0}, {1}) == true);",
	"MATH_STRING_NOT_EQUAL": "this->push(stringCompareInsensitive({0}, {1}) == false);",
	"MATH_APPEND": """size_t firstSize = strlen({0}), secondSize = strlen({1});
			char* stringResult = new char[firstSize + secondSize + 1];
			strncpy(stringResult, {0}, firstSize);
			strncpy(&stringResult[firstSize], {1}, secondSize);
			stringResult[firstSize + secondSize] = '\\0';

			%%popStrings%%

			this->push(stringResult);""",
	"MATH_SPC": """size_t firstSize = strlen({0}), secondSize = strlen({1});
			char* stringResult = new char[firstSize + secondSize + 2];
			strncpy(stringResult, {0}, firstSize);
			stringResult[firstSize] = ' ';
			strncpy(&stringResult[firstSize + 1], {1}, secondSize);
			stringResult[firstSize + secondSize + 1] = '\\0';

			%%popStrings%%
			
			this->push(stringResult);""",
	"MATH_TAB": """size_t firstSize = strlen({0}), secondSize = strlen({1});
			char* stringResult = new char[firstSize + secondSize + 2];
			strncpy(stringResult, {0}, firstSize);
			stringResult[firstSize] = '\\t';
			strncpy(&stringResult[firstSize + 1], {1}, secondSize);
			stringResult[firstSize + secondSize + 1] = '\\0';

			%%popStrings%%

			this->push(stringResult);""",
	"MATH_NL": """size_t firstSize = strlen({0}), secondSize = strlen({1});
			char* stringResult = new char[firstSize + secondSize + 2];
			strncpy(stringResult, {0}, firstSize);
			stringResult[firstSize] = '\\n';
			strncpy(&stringResult[firstSize + 1], {1}, secondSize);
			stringResult[firstSize + secondSize + 1] = '\\0';

			%%popStrings%%

			this->push(stringResult);""",
}

number_typecheck = """if(%%.type == entry::NUMBER) {
						%%Number = %%.numberData;
					}
					else if(%%.type == entry::OBJECT) {
						if(%%.objectData->object == nullptr) {
							%%Number = 0.0;
							this->warning("trying to access deleted object\\n");
						}
						else {
							%%Number = %%.objectData->object->id;
						}
					}
					else {
						%%Number = stringToNumber(%%.stringData);
					}"""

string_typecheck = """if(%%->type == entry::STRING) {
					%%String = %%->stringData;
				}
				else if(%%->type == entry::OBJECT) {
					if(%%->objectData->object == nullptr) {
						%%String = getEmptyString();
						this->warning("trying to access deleted object\\n");
					}
					else {
						%%String = numberToString(%%->objectData->object->id);
					}
					delete%%String = true;
				}
				else {
					%%String = numberToString(%%->numberData);
					delete%%String = true;
				}"""

string_pop = """if(popLValue) {{
				this->pop();
			}}

			if(popRValue) {{
				this->pop();
			}}"""

lvalue_number_typecheck = number_typecheck.replace("%%", "lvalue")
rvalue_number_typecheck = number_typecheck.replace("%%", "rvalue")
NUMBER_MATH_MACRO = get_generated_code("math", "numbers", 3)
NUMBER_MATH_MACRO = NUMBER_MATH_MACRO.replace("%%lvalueTypecheck%%", lvalue_number_typecheck)
NUMBER_MATH_MACRO = NUMBER_MATH_MACRO.replace("%%rvalueTypecheck%%", rvalue_number_typecheck)

# handle number instructions
for instruction, operation in number_operations.items():
	formatted = operation.format("lvalueNumber", "rvalueNumber")

	print(f"""		case instruction::{instruction}: {{
{NUMBER_MATH_MACRO}

			{formatted}
			break;
		}}\n""")

lvalue_string_typecheck = string_typecheck.replace("%%", "lvalue")
rvalue_string_typecheck = string_typecheck.replace("%%", "rvalue")
STRING_MATH_MACRO = get_generated_code("math", "strings", 3)
STRING_MATH_MACRO = STRING_MATH_MACRO.replace("%%lvalueTypecheck%%", lvalue_string_typecheck)
STRING_MATH_MACRO = STRING_MATH_MACRO.replace("%%rvalueTypecheck%%", rvalue_string_typecheck)

# handle string instructions
for instruction, operation in string_operations.items():
	formatted = operation.format("lvalueString", "rvalueString")
	DELETE_STRINGS_MACRO = get_generated_code("math", "deleteStrings", 3)

	print(f"""		case instruction::{instruction}: {{
{STRING_MATH_MACRO}

			{formatted.replace("%%popStrings%%", string_pop)}
{DELETE_STRINGS_MACRO}
			break;
		}}\n""")