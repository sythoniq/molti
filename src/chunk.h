// So a chunk is a sequence of bytecode

#ifndef MOLTI_CHUNK_H
#define MOLTI_CHUNK_H

#include "common.h"
#include "memory.h"
#include "value.h"

/*
- Using typedef to work as an alias and remove the need to repeat enum on every call of a enum variable/instance. 
- Each value within the enum is an instruction = operation code so OP_RETURN is a return instruction ie, return from this function.
*/

typedef enum {
				OP_CONSTANT,
				OP_RETURN
} OpCode;

/* 
- So declaring a dynamic array that will contain chunks of byte code and it will expand accordingly whether there is enough space ie (capacity > count) if not then; allocate a new array and copy the contents of the previous array and ensure the new array has double the capacity.
*/

typedef struct {
				int capacity;
				int count;
				uint8_t *code;
				int* lines;
				ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);

// Write a byte to a chunk
void writeChunk(Chunk *chunk, uint8_t byte, int line);

// Free memory
void freeChunk(Chunk *chunk);

int addConstant(Chunk *chunk, Value value);

#endif
