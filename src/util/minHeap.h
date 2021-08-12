#pragma once

#include "dynamicArray.h"

template <typename T, typename S>
class MinHeap {
	public:
		MinHeap() {

		}
		
		MinHeap(
			S* parent,
			void (*init) (S* parent, T* location),
			void (*onRealloc) (S* parent)
		) {
			new((void*)&this->array) DynamicArray<T, S>(parent, 4, init, onRealloc);
		}

		void insert(T value) {
			// insert at the end
			size_t iterator = this->array.head;
			this->array[iterator] = value;
			this->array.pushed();

			while(iterator != 0 && this->compare(iterator, this->parentIndex(iterator))) {
				this->swap(iterator, this->parentIndex(iterator));
				iterator = this->parentIndex(iterator);
			}
		}

		T& top() {
			return this->array[0];
		}

		void pop() {
			if(this->array.head == 1) {
				this->array.popped();
			}
			else {
				this->array[0] = this->array[this->array.head - 1];
				this->array.popped();

				this->organize(0);
			}
		}
	
		DynamicArray<T, S> array;
	
	private:
		void (*init) (S* parent, T* location);
		void (*onRealloc) (S* parent);

		size_t parentIndex(size_t index) {
			return (index - 1) / 2;
		}

		size_t leftChildIndex(size_t index) {
			return (2 * index + 1);
		}

		size_t rightChildIndex(size_t index) {
			return (2 * index + 2);
		}

		void swap(size_t index1, size_t index2) {
			T temp = this->array[index1];
			this->array[index1] = this->array[index2];
			this->array[index2] = temp;
		}

		void organize(size_t index) {
			size_t leftIndex = this->leftChildIndex(index);
			size_t rightIndex = this->rightChildIndex(index);
			size_t smallestIndex = index;

			if(leftIndex < this->array.head && this->compare(leftIndex, smallestIndex)) {
				smallestIndex = leftIndex;
			}

			if(rightIndex < this->array.head && this->compare(rightIndex, smallestIndex)) {
				smallestIndex = rightIndex;
			}

			if(smallestIndex != index) {
				this->swap(index, smallestIndex);
				this->organize(smallestIndex);
			}
		}

		bool compare(size_t leftIndex, size_t rightIndex) {
			if(std::is_pointer<T>::value) {
				return *(this->array[leftIndex]) < *(this->array[rightIndex]);
			}
			else {
				return this->array[leftIndex] < this->array[rightIndex];
			}
		}
};
