#include "parser.h"

Parser::Parser(Tokenizer* tokenizer, ParsedArguments args) {
	this->newLine = "\n";
	this->tab = "\t";
	this->space = " ";
	if(args.arguments["minify"] != "") {
		this->newLine = "";
		this->tab = "";
		this->space = "";
		this->minified = true;
	}

	string outputPath = tokenizer->fileName;
	if(args.arguments["output"] != "") {
		string directory = args.arguments["output"];
		if(directory[directory.length() - 1] != '/') {
			directory += '/';
		}
		outputPath = directory + outputPath;
	}
	
	this->tokenizer = tokenizer;
	this->sourceFile = new SourceFile(this);
	Component::ParseBody(this->sourceFile, this->tokenizer, this);

	// don't overwrite original file
	if(args.arguments["piped"] != "") {
		printf("%s\n", this->sourceFile->print().c_str());
	}
	else {
		if(outputPath != tokenizer->fileName) {
			filesystem::create_directories(filesystem::path(outputPath).remove_filename());
			ofstream file(outputPath);
			file << this->sourceFile->print();
			file.close();
		}
	}
}

Token Parser::expectToken(TokenType type1, TokenType type2, TokenType type3, TokenType type4, TokenType type5) {
	bool foundType = false;
	Token token = this->tokenizer->getToken();
	
	if(token.type == type1) {
		foundType = true;
	}
	else if(token.type == type2 && type2 != INVALID) {
		foundType = true;
	}
	else if(token.type == type3 && type3 != INVALID) {
		foundType = true;
	}
	else if(token.type == type4 && type4 != INVALID) {
		foundType = true;
	}
	else if(token.type == type5 && type5 != INVALID) {
		foundType = true;
	}

	if(!foundType) {
		this->error("unexpected token '%s' found, wanted %s", token.lexeme.c_str(), this->tokenizer->typeToName(type1));
	}

	return token;
}

SourceFile* Parser::getSourceFile() {
	return this->sourceFile;
}