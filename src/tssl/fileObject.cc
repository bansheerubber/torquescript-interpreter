#include "fileObject.h"

#include "../util/cloneString.h"
#include "../engine/engine.h"
#include "../util/getEmptyString.h"
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

			if(output.length() > 0 && output[output.length() - 1] == '\r') {
				output.pop_back();
			}

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

		Entry* FileObject__openForRead(Engine* engine, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, READ);
			}
			
			return nullptr;
		}

		Entry* FileObject__openForWrite(Engine* engine, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, WRITE);
			}
			
			return nullptr;
		}

		Entry* FileObject__openForAppend(Engine* engine, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->open(args[1].stringData, APPEND);
			}
			
			return nullptr;
		}

		Entry* FileObject__close(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->close();
			}
			
			return nullptr;
		}

		Entry* FileObject__readLine(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				return new Entry(((FileObject*)args[0].objectData->objectWrapper->data)->readLine());
			}
			
			return nullptr;
		}

		Entry* FileObject__writeLine(Engine* engine, size_t argc, Entry* args) {
			if(argc == 2 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				((FileObject*)args[0].objectData->objectWrapper->data)->writeLine(args[1].stringData);
			}
			
			return nullptr;
		}

		Entry* FileObject__isEOF(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1 && args[0].objectData->objectWrapper->object->typeMethodTree->name == "FileObject") {
				return new Entry((double)((FileObject*)args[0].objectData->objectWrapper->data)->isEOF());
			}
			
			return nullptr;
		}

		Entry* fileBase(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* path = args[0].stringData;
				const char* slashLocation = strrchr(path, '/');
				if(slashLocation == nullptr) {
					slashLocation = path;
				}
				else {
					slashLocation++;
				}

				const char* dotLocation = strrchr(path, '.');
				
				size_t length = dotLocation
					? dotLocation - slashLocation
					: strlen(slashLocation);
				
				char* newString = new char[length + 1];
				strncpy(newString, slashLocation, length);
				newString[length] = '\0';
				
				return new Entry(newString);
			}

			return nullptr;
		}

		Entry* fileExt(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* path = args[0].stringData;
				const char* dotLocation = strrchr(path, '.');
				if(dotLocation == nullptr) {
					return new Entry(getEmptyString());
				}

				return new Entry(cloneString(dotLocation));
			}

			return nullptr;
		}

		Entry* fileName(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* path = args[0].stringData;
				const char* slashLocation = strrchr(path, '/');
				if(slashLocation == nullptr) {
					slashLocation = path;
				}
				else {
					slashLocation++;
				}

				return new Entry(cloneString(slashLocation));
			}

			return nullptr;
		}

		Entry* filePath(Engine* engine, size_t argc, Entry* args) {
			if(argc == 1) {
				const char* path = args[0].stringData;
				const char* slashLocation = strrchr(path, '/');
				if(slashLocation == nullptr) {
					return new Entry(cloneString(path));
				}

				size_t length = slashLocation - path;
				char* newString = new char[length + 1];
				strncpy(newString, path, length);
				newString[length] = '\0';
				
				return new Entry(newString);
			}

			return nullptr;
		}
	}
}
