#include "tokenizer.h"

bool Tokenizer::isWhitespace(char character) {
	return character == ' ' || character == '\n' || character == '\r' || character == '\t';
}

int Tokenizer::getLineNumber(int offset) {
	return this->info[this->fileIndex - offset].lineNumber;
}

int Tokenizer::getCharacterNumber(int offset) {
	return this->info[this->fileIndex - offset].characterNumber;
}

char Tokenizer::getChar() {
	char output;
	if(this->isPiped) {
		if(this->fileIndex >= this->pipedFile.length()) {
			this->overrun++;
			return '\0';
		}
		
		output = this->pipedFile[this->fileIndex];
		this->fileIndex++;
	}
	else {
		this->file.get(output);
		this->fileIndex++;

		// if we overrun the file, just imagine that we're still reading but just blank bytes
		if(this->file.eof()) {
			this->overrun++;
			return '\0';
		}
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
		if(this->isPiped) {
			this->fileIndex--;
		}
		else {
			this->file.clear();
			this->file.unget();
			this->fileIndex--;
		}
	}
}

int Tokenizer::getLineCount() {
	return this->lastValidLineNumber;
}

int Tokenizer::getCharacterCount() {
	return this->info.size();
}

bool Tokenizer::isFileEOF() {
	if(this->isPiped) {
		return this->fileIndex >= this->pipedFile.size();
	}
	else {
		return !this->file.good();
	}
}