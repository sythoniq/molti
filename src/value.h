#ifndef MOLTI_VALUE_H
#define MOLTI_VALUE_H

typedef double Value;

typedef struct {
				int capacity;
				int count;
				Value *values;
} ValueArray;

void initValueArray(ValueArray *arr);
void writeValueArray(ValueArray *arr, Value value);
void printValue(Value value);
void freeValueArray(ValueArray *arr);

#endif
