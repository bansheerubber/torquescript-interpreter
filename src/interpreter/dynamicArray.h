#pragma once

#include <cstring>

#include "../io.h"

namespace ts {
	#define DYNAMIC_ARRAY_MAX_SIZE 5000000
	
	template <typename T, typename S>
	struct DynamicArray {
		T* array;
		size_t size;
		size_t head;
		S* parent;
		void (*init) (S* parent, T* location);
		void (*onRealloc) (S* parent);

		DynamicArray() {
			this->array = nullptr;
		}

		DynamicArray(
			S* parent,
			size_t size,
			void (*init) (S* parent, T* location),
			void (*onRealloc) (S* parent)
		) {
			this->parent = parent;
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
				(*this->init)(this->parent, &this->array[i]);
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
					(*this->init)(this->parent, &this->array[i]);
				}
				this->size *= 2;
				
				if(this->onRealloc != nullptr) {
					(*this->onRealloc)(this->parent);
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
