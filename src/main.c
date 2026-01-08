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
  writeChunk(&chunk, OP_CONSTANT, 123); // Write the instruction to the stack.
  writeChunk(&chunk, constant, 123); // Push the constant onto the stack afterwards.

  writeChunk(&chunk, OP_RETURN, 123); // Push the last instructions to the stack

  // Stack in this case == chunk->code

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
