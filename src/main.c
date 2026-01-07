#include <stdio.h>

#include "./headers/common.h"
#include "./headers/chunk.h"
#include "./headers/debug.h"

#include "memory.c"
#include "debug.c"
#include "chunk.c"
#include "value.c"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  
  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT);
  writeChunk(&chunk, constant);

  writeChunk(&chunk, OP_RETURN);

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
