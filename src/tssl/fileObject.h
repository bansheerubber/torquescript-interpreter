#pragma once

#include <fstream>
#include <string>

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

using namespace std;

namespace ts {
	class Interpreter;

	namespace sl {
		enum FileObjectMode {
			NOT_OPEN,
			READ,
			WRITE,
			APPEND,
		};
		
		class FileObject {
			public:
				void open(const char* fileName, FileObjectMode mode);
				void close();
				char* readLine();
				void writeLine(const char* string);
				bool isEOF();

			private:
				fstream file;
				FileObjectMode mode = NOT_OPEN;
		};

		void FileObject__constructor(ObjectWrapper* wrapper);
		Entry* FileObject__openForRead(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__openForWrite(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__openForAppend(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__close(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__readLine(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__writeLine(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* FileObject__isEOF(Interpreter* interpreter, size_t argc, Entry* args);

		Entry* fileBase(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* fileExt(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* fileName(Interpreter* interpreter, size_t argc, Entry* args);
		Entry* filePath(Interpreter* interpreter, size_t argc, Entry* args);
	}
}