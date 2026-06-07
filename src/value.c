#include "common.h"
#include "value.h"
#include "memory.h"

void initValueArray(ValueArray *arr) {
				arr->capacity = 0;
				arr->count = 0;
				arr->values = NULL;
}

void writeValueArray(ValueArray *arr, Value value) {
				if (arr->capacity < arr->count + 1) {
								int oldCap = arr->capacity;
								arr->capacity = GROW_CAPACITY(oldCap);
								arr->values = GROW_ARRAY(Value, arr->values, oldCap, arr->capacity);
				}

				arr->values[arr->count] = value;
				arr->count++;
}

void printValue(Value value) {
				printf("%g", value);
}

void freeValueArray(ValueArray *arr) {
				FREE_ARRAY(Value, arr->values, arr->capacity);
				initValueArray(arr);
}
