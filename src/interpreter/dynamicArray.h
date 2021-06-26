#pragma once

#include <stdlib.h>
#include <cstring>

#include "../io.h"

namespace ts {
	template <typename T>
	struct DynamicArray {
		T* array;
		int size;
		int maxSize;
		int head;
		class Interpreter* interpreter;
		void (*init) (class Interpreter* interpreter, T* location);
		void (*onRealloc) (class Interpreter* interpreter);

		DynamicArray() {

		}

		DynamicArray(
			class Interpreter* interpreter,
			int size,
			int maxSize,
			void (*init) (class Interpreter* interpreter, T* location),
			void (*onRealloc) (class Interpreter* interpreter)
		) {
			this->interpreter = interpreter;
			this->size = size;
			this->maxSize = maxSize;
			this->head = 0;
			this->array = (T*)malloc(sizeof(T) * this->size);
			this->init = init;
			this->onRealloc = onRealloc;

			for(int i = 0; i < this->size; i++) {
				(*this->init)(this->interpreter, &this->array[i]);
			}
		}

		void pushed() {
			this->head++;

			if(this->head == this->size) {
				if(this->size * 2 > this->maxSize) {
					printError("stack overflow\n");
					exit(1);
				}
				
				this->array = (T*)realloc(this->array, sizeof(T) * this->size * 2);
				for(int i = this->size; i < this->size * 2; i++) {
					(*this->init)(this->interpreter, &this->array[i]);
				}
				this->size *= 2;
				
				if(this->onRealloc != nullptr) {
					(*this->onRealloc)(this->interpreter);
				}
			}
		}

		void popped() {
			this->head--;
		}
	};
}
