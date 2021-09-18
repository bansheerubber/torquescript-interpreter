#pragma once

#include <fstream>
#include <string>

#include "../interpreter/entry.h"
#include "../interpreter/object.h"

using namespace std;

namespace ts {
	class Engine;

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
		Entry* FileObject__openForRead(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__openForWrite(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__openForAppend(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__close(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__readLine(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__writeLine(Engine* engine, unsigned int argc, Entry* args);
		Entry* FileObject__isEOF(Engine* engine, unsigned int argc, Entry* args);

		Entry* fileBase(Engine* engine, unsigned int argc, Entry* args);
		Entry* fileExt(Engine* engine, unsigned int argc, Entry* args);
		Entry* fileName(Engine* engine, unsigned int argc, Entry* args);
		Entry* filePath(Engine* engine, unsigned int argc, Entry* args);
	}
}