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

void initChunk(Chunk *chunk) {
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

				if (chunk->lineCount > 0 && chunk->lines[chunk->count - 1].line == line) 
								return;

				if (chunk->lineCapacity < chunk->lineCount + 1) {
								int oldCap = chunk->lineCapacity;
								chunk->lineCapacity = GROW_CAPACITY(oldCap);
								chunk->lines = GROW_ARRAY(Lines, chunk->lines, oldCap, chunk->lineCapacity);
				}

				Lines *lines = &chunk->lines[chunk->lineCount + 1];
				lines->offset = chunk->count - 1;
				lines->line = line;
}

void freeChunk(Chunk *chunk) {
				FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
				FREE_ARRAY(int, chunk->lines, chunk->lineCapacity);
				freeValueArray(&chunk->constants);
				initChunk(chunk);
}

// Returning offset - 1, since we need to locate where exactly the constant was added ie its actual index within the constant array, fancy.

int addConstant(Chunk *chunk, Value value) {
				writeValueArray(&chunk->constants, value);
				return chunk->constants.count - 1;
}
