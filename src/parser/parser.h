#pragma once

#include "../tokenizer/tokenizer.h"

class Parser {
	public:
		Parser(Tokenizer* tokenizer);
	
	private:
		Tokenizer* tokenizer;
};