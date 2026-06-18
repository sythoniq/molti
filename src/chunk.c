#include "chunk.h"
#include "memory.h"
#include "value.h"

int getLine(Chunk *chunk, int instruction) {
				int start = 0;
				int end = chunk->lineCount - 1;

				for (;;) { // Basically a while loop, a infinite loop until a break
								int mid = (start + end) / 2;
								LineStart *line = &chunk->lines[mid];
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
								chunk->lines = GROW_ARRAY(LineStart, chunk->lines, oldCap, chunk->lineCapacity);
				}

				LineStart *lineStart = &chunk->lines[chunk->lineCount++];
				lineStart->offset = chunk->count - 1;
				lineStart->line = line;
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

// If the current index of the value within the constants array is more than 256 (more than a byte), the code array that holds our instructions/opcodes as well as a pointer to the constants array after an instruction wont be able to handle it as such we break down the index using little endian (least significant byte first) and pass them to the code array for later referencing to the constants array.
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
