#include "chunk.h"
#include "memory.h"
#include "value.h"

int getLine(Chunk *chunk, int instruction) {
				int start = 0;
				int end = chunk->lineCount - 1;

				for (;;) {
								int mid = (start + end) / 2;
								Lines *line = &chunk->lines[mid];
								if (instruction < line->offset) {
												end = mid - 1;
								} else if (mid == chunk->lineCount - 1 ||
												instruction < chunk->lines[mid + 1].offset) {
												return line->line;
								} else {
												start = mid + 1;
								}
				}
}

void initValueChunk(Chunk *chunk) {
				chunk->capacity = 0;
				chunk->count = 0;
				chunk->code = NULL;
				chunk->lines = NULL;
				chunk->lineCount = 0;
				chunk->lineCapacity = 0;
				initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
				if (chunk->capacity < chunk->count + 1) {
								int oldCap = chunk->capacity;
								chunk->capacity = GROW_CAPACITY(oldCap);
								chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCap, chunk->capacity);
				}
				chunk->code[chunk->count] = byte;
				chunk->count++;

				if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount - 1].line == line) 
								return;

				if (chunk->lineCapacity < chunk->lineCount + 1) {
								int oldCap = chunk->lineCapacity;
								chunk->lineCapacity = GROW_CAPACITY(oldCap);
								chunk->lines = GROW_ARRAY(Lines, chunk->lines, oldCap, chunk->lineCapacity);
				}

				Lines *lines = &chunk->lines[chunk->lineCount++];
				lines->offset = chunk->count - 1;
				lines->line = line;
}

void freeValueChunk(Chunk *chunk) {
				FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
				FREE_ARRAY(int, chunk->lines, chunk->lineCapacity);
				freeValueArray(&chunk->constants);
				initValueChunk(chunk);
}

// Returning offset - 1, since we need to locate where exactly the constant was added ie its actual constantIndex within the constant array, fancy.

int addConstant(Chunk *chunk, Value value) {
				writeValueArray(&chunk->constants, value);
				return chunk->constants.count - 1;
}

// If the constantIndex fits in one byte we write it to the normal op constant else we write it to long const tho the value is split into multiple bytes and the endianness choses here is little endian ie the least significant (lowest value) byte of the long byte comes first

// We right shift the constantIndex to get the least significant bit from the byte of code (long const)

// This quite literally allows us to access chunks past 255 within the constants array since it only takes one byte as its index
void writeConstant(Chunk *chunk, Value value, int line) {
				// Adding the value to the constant array and getting its constantIndex/position in the array.
				int constantIndex = addConstant(chunk, value);
				printf("Constant Index: %d\n", constantIndex);
				if (constantIndex < 256) {
								writeChunk(chunk, OP_CONSTANT, line);
								writeChunk(chunk, (uint8_t)constantIndex, line);
				} else {
								writeChunk(chunk, OP_CONSTANT_LONG, line);
								writeChunk(chunk, (uint8_t)(constantIndex & 0xff), line);
								writeChunk(chunk, (uint8_t)((constantIndex >> 8) & 0xff), line);
								writeChunk(chunk, (uint8_t)((constantIndex >> 16) & 0xff), line);
				}
}
