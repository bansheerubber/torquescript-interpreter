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
	this->fileIndex++;

	// if we overrun the file, just imagine that we're still reading but just blank bytes
	if(this->file.eof()) {
		this->overrun++;
		return '\0';
	}

	if(this->infoSize == 0) {
		this->info.push_back({
			lineNumber: 1,
			characterNumber: 1,
		});
		this->infoSize++;
	}
	else if(this->infoSize == this->fileIndex - 1) {
		int lineNumber = output != '\n'
			? this->info[this->fileIndex - 2].lineNumber
			: this->info[this->fileIndex - 2].lineNumber + 1;
		int characterNumber = output != '\n'
			? this->info[this->fileIndex - 2].characterNumber + 1
			: 0;

		this->lastValidLineNumber = lineNumber;
		this->lastValidCharacterNumber = characterNumber;
		
		this->info.push_back({
			lineNumber: lineNumber,
			characterNumber: characterNumber,
		});
		this->infoSize++;
	}

	return output;
}

void Tokenizer::prevChar() {
	if(this->overrun > 0) {
		this->overrun--;
	}
	else {
		this->file.clear();
		this->file.unget();
		this->fileIndex--;
	}
}

int Tokenizer::getLineCount() {
	return this->lastValidLineNumber;
}

int Tokenizer::getCharacterCount() {
	return this->info.size();
}