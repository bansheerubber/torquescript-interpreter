#include "tokenizer.h"

bool Tokenizer::isWhitespace(char character) {
	return character == ' ' || character == '\n' || character == '\t';
}

char Tokenizer::getChar() {
	char output;

	// if we overrun the file, just imagine that we're still reading but just blank bytes
	if(this->fileIndex >= this->contentSize) {
		this->overrun++;
		return '\0';
	}
	
	output = this->contents[this->fileIndex];
	this->fileIndex++;

	if(output == '\n') {
		this->lineNumber++;
		this->characterNumber = 1;
	}
	else {
		this->characterNumber++;
	}

	// skip past all carriage returns
	if(output == '\r') {
		return this->getChar();
	}

	return output;
}

void Tokenizer::prevChar() {
	if(this->overrun > 0) {
		this->overrun--;
	}
	else {
		this->fileIndex--;

		if(this->fileIndex < this->contentSize && this->contents[this->fileIndex] == '\n') {
			// count the amount of characters on this line
			// TODO does this affect parsing performance significantly?
			int count = 0;
			for(int i = this->fileIndex - 1; i >= 0; i--) {
				if(this->contents[i] == '\n') {
					break;
				}
				count++;
			}
			this->lineNumber--;
			this->characterNumber = count;
		}
		else {
			this->characterNumber--;
		}

		// skip past all carriage returns
		if(this->contents[this->fileIndex] == '\r') {
			this->prevChar();
		}
	}
}

size_t Tokenizer::getTotalLineCount() {
	return this->lineNumber;
}

size_t Tokenizer::getTotalCharacterCount() {
	return this->contentSize;
}

bool Tokenizer::isFileEOF() {
	return this->fileIndex >= this->contentSize;
}