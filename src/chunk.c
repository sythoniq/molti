#include <stdlib.h>
#include "./headers/chunk.h"
#include "./headers/memory.h"
#include "./headers/value.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity); // Basically the type of
  // the array and then the pointer to the array ie chunk->code and the current
  // capacity/size which will be oldCap in the header/reallocate func
  freeValueArray(&chunk->constants);
  initChunk(chunk); // Zero out everything and create an empty state.
}

void writeChunk(Chunk* chunk, uint8_t byte) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity,
                             chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}
