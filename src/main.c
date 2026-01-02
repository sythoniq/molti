#include "./headers/common.h"
#include "./headers/chunk.h"
#include <stdio.h>

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN);
  freeChunk(&chunk);
  return 0;
}
