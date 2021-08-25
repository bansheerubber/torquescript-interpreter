#include "fileObject.h"

#include "../util/getEmptyString.h"
#include "../interpreter/interpreter.h"
#include "../util/stringToChars.h"

namespace ts {
	namespace sl {
		void FileObject::open(const char* fileName, FileObjectMode mode) {
			ios_base::openmode fileMode;
			if(mode == READ) {
				fileMode = ios_base::in;
			}
			else if(mode == WRITE) {
				fileMode = ios_base::trunc;
			}
			else if(mode == APPEND) {
				fileMode = ios_base::app;
			}

			this->file.open(string(fileName), fileMode);
			this->mode = mode;
		}

		void FileObject::close() {
			this->file.close();
		}

		char* FileObject::readLine() {
			if(this->mode != READ || this->isEOF()) {
				return getEmptyString();
			}
			
			string output;
			getline(this->file, output);
			return stringToChars(output);
		}

		void FileObject::writeLine(const char* string) {
			if(this->mode == READ || this->mode == NOT_OPEN) {
				return;
			}

			this->file.write(string, strlen(string));
		}

		bool FileObject::isEOF() {
			return this->file.eof();
		}
		
		void FileObject__constructor(ObjectWrapper* wrapper) {
			wrapper->data = new FileObject();
		}

		Entry* FileObject__openForRead(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, READ);
			}
			
			return nullptr;
		}

		Entry* FileObject__openForWrite(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, WRITE);
			}
			
			return nullptr;
		}

		Entry* FileObject__openForAppend(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, APPEND);
			}
			
			return nullptr;
		}

		Entry* FileObject__close(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->close();
			}
			
			return nullptr;
		}

		Entry* FileObject__readLine(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				return new Entry(((FileObject*)args[0].objectData->objectWrapper->data)->readLine());
			}
			
			return nullptr;
		}

		Entry* FileObject__writeLine(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->writeLine(args[1].stringData);
			}
			
			return nullptr;
		}

		Entry* FileObject__isEOF(Interpreter* interpreter, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				return new Entry((double)((FileObject*)args[0].objectData->objectWrapper->data)->isEOF());
			}
			
			return nullptr;
		}
	}
}
