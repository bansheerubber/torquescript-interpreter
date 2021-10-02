#pragma once

enum ComponentType {
	INVALID_STATEMENT,
	SOURCE_FILE,
	IF_STATEMENT,
	ELSE_STATEMENT,
	ELSE_IF_STATEMENT,
	WHILE_STATEMENT,
	FOR_STATEMENT,
	SWITCH_STATEMENT,
	CASE_STATEMENT,
	DEFAULT_STATEMENT,
	ASSIGN_STATEMENT,
	ACCESS_STATEMENT,
	ARRAY_STATEMENT,
	CALL_STATEMENT,
	NUMBER_LITERAL,
	STRING_LITERAL,
	BOOLEAN_LITERAL,
	MATH_EXPRESSION,
	POSTFIX_STATEMENT,
	PARENT_STATEMENT,
	RETURN_STATEMENT,
	FUNCTION_DECLARATION,
	DATABLOCK_DECLARATION,
	SYMBOL_STATEMENT,
	INHERITANCE_STATEMENT,
	PACKAGE_DECLARATION,
	COMMENT_STATEMENT,
	INLINE_CONDITIONAL,
	NEW_STATEMENT,
	CONTINUE_STATEMENT,
	BREAK_STATEMENT,
};