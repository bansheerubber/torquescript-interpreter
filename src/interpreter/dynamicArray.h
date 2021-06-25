#pragma once

#include <stdlib.h>
#include <cstring>

namespace ts {
	template <typename T>
	struct DynamicArray {
		T* array;
		int maxSize;
		int head;
		void (*init) (T* location);

		DynamicArray(int maxSize, void (*init) (T* location)) {
			this->maxSize = maxSize;
			this->head = 0;
			this->array = (T*)malloc(sizeof(T) * this->maxSize);
			this->init = init;

			for(int i = 0; i < this->maxSize; i++) {
				(*this->init)(&this->array[i]);
			}
		}

		void pushed() {
			this->head++;

			if(this->head == this->maxSize) {
				T* oldArray = this->array;
				this->array = (T*)realloc(this->array, sizeof(T) * this->maxSize * 2);
				for(int i = this->maxSize; i < this->maxSize * 2; i++) {
					(*this->init)(&this->array[i]);
				}
				this->maxSize *= 2;
			}
		}

		void popped() {
			this->head--;
		}
	};
}
