#include "chunk.h"

void initChunk(Chunk *chunk) {
				chunk->capacity = 0;
				chunk->count = 0;
				chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
				if (chunk->capacity < chunk->count + 1) {
								int oldCap = chunk->capacity;
								chunk->capacity = GROW_CAPACITY(oldCap);
								chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCap, chunk->capacity);
				}
				chunk->code[chunk->count] = byte;
				chunk->count++;
}
