#ifndef MOLTI_VM_H
#define MOLTI_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
				Chunk* chunk;
				// Create a instruction pointer of type byte
				uint8_t* ip; // This is to track where the vm is currently at, would be better to have as a local variable for C to keep it in a register as it changes quite often.
				// In C its faster to dereference a pointer than to have a int index of an array to find where exactly the VM is at within the array of bytes (Chunk)
				Value stack[STACK_MAX];
				Value *stackTop;
} VM;

// Enum = named int constant, helps just make sure that the values/data type of int has a understable/readable name.

typedef enum {
				INTERPRET_OK,
				INTERPRET_COMPILE_ERROR,
				INTERPRET_RUNTIME_ERROR
} InterpreterResult;

void initializeVM();
void freeVM();

void push(Value value);
Value pop();

InterpreterResult interpret(Chunk *chunk);
#endif
