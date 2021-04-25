#pragma once

enum ComponentType {
	INVALID_STATEMENT,
	SOURCE_FILE,
	IF_STATEMENT,
	WHILE_STATEMENT,
	FOR_STATEMENT,
	SWITCH_STATEMENT,
	ASSIGN_STATEMENT,
};

class Component {
	virtual ComponentType show() = 0;
};