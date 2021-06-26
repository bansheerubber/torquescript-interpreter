#pragma once

#include <stdlib.h>
#include <cstring>

#include "../io.h"

namespace ts {
	#define DYNAMIC_ARRAY_MAX_SIZE 5000000
	
	template <typename T>
	struct DynamicArray {
		T* array;
		size_t size;
		size_t head;
		class Interpreter* interpreter;
		void (*init) (class Interpreter* interpreter, T* location);
		void (*onRealloc) (class Interpreter* interpreter);

		DynamicArray() {
			this->array = nullptr;
		}

		DynamicArray(
			class Interpreter* interpreter,
			int size,
			void (*init) (class Interpreter* interpreter, T* location),
			void (*onRealloc) (class Interpreter* interpreter)
		) {
			this->interpreter = interpreter;
			this->size = size;
			this->head = 0;
			this->init = init;
			this->onRealloc = onRealloc;

			T* array = (T*)malloc(sizeof(T) * this->size);
			if(array == NULL) {
				printError("invalid dynamic array malloc\n");
				exit(1);
			}
			this->array = array;

			for(size_t i = 0; i < this->size; i++) {
				(*this->init)(this->interpreter, &this->array[i]);
			}
		}

		void pushed() {
			this->head++;

			if(this->head == this->size) {
				if(this->size * 2 > DYNAMIC_ARRAY_MAX_SIZE) {
					printError("stack overflow\n");
					exit(1);
				}
				
				T* array = (T*)realloc(this->array, sizeof(T) * this->size * 2);
				if(array == NULL) {
					printError("invalid dynamic array realloc\n");
					exit(1);
				}
				this->array = array;

				for(size_t i = this->size; i < this->size * 2; i++) {
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

		T& operator[](size_t index) {
			return this->array[index];
		}
	};
}
