#include "tokenizer.h"

bool Tokenizer::isWhitespace(char character) {
	return character == ' ' || character == '\n' || character == '\r' || character == '\t';
}

int Tokenizer::getLineNumber(int offset) {
	return this->info[(int)this->file.tellg() - offset].lineNumber;
}

int Tokenizer::getCharacterNumber(int offset) {
	return this->info[(int)this->file.tellg() - offset].characterNumber;
}

char Tokenizer::getChar() {
	char output;
	this->file.get(output);

	// if we overrun the file, just imagine that we're still reading but just blank bytes
	if(this->file.eof()) {
		this->overrun++;
		return '\0';
	}

	if(this->info.size() == 0) {
		this->info.push_back({
			character: output,
			lineNumber: 1,
			characterNumber: 1,
		});
	}
	else if((int)this->info.size() == (int)this->file.tellg() - 1) {
		int lineNumber = output != '\n'
			? this->info[(int)this->file.tellg() - 2].lineNumber
			: this->info[(int)this->file.tellg() - 2].lineNumber + 1;
		int characterNumber = output != '\n'
			? this->info[(int)this->file.tellg() - 2].characterNumber + 1
			: 0;

		this->lastValidLineNumber = lineNumber;
		this->lastValidCharacterNumber = characterNumber;
		
		this->info.push_back({
			character: output,
			lineNumber: lineNumber,
			characterNumber: characterNumber,
		});
	}

	return output;
}

void Tokenizer::prevChar() {
	// TODO update line number and character number

	if(this->overrun > 0) {
		this->overrun--;
	}
	else {
		this->file.clear();
		this->file.unget();
	}
}