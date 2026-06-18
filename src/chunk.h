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
				OP_CONSTANT_LONG,
				OP_ADD,
				OP_SUBTRACT,
				OP_DIVIDE,
				OP_MULTIPLY,
				OP_NEGATE,
				OP_RETURN
} OpCode;

typedef struct {
				int offset;
				int line;
} LineStart;

/* 
- So declaring a dynamic array that will contain chunks of byte code and it will expand accordingly whether there is enough space ie (capacity > count) if not then; allocate a new array and copy the contents of the previous array and ensure the new array has double the capacity.
*/

typedef struct {
				int capacity;
				int count;
				uint8_t *code;
				int lineCount;
				int lineCapacity;
				LineStart *lines;
				ValueArray constants;
} Chunk;

int getLine(Chunk *chunk, int instruction);

void initValueChunk(Chunk *chunk);

// Write a byte to a chunk
void writeChunk(Chunk *chunk, uint8_t byte, int line);

// Free memory
void freeValueChunk(Chunk *chunk);

int addConstant(Chunk *chunk, Value value);

// Helper function to write all consants while checking index to decide whether its a long const or a normal one (OP_CONSTANT and OP_CONSTANT_LONG)
void writeConstant(Chunk *chunk, Value value, int line);

#endif
