#include "./common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
				Chunk chunk;
				initializeVM();
				initValueChunk(&chunk);
				writeConstant(&chunk, 100, 1);
				writeChunk(&chunk, OP_NEGATE, 1);
				writeChunk(&chunk, OP_RETURN, 2);
				disassembleChunk(&chunk, "test chunk");
				interpret(&chunk);
				freeVM();
				freeValueChunk(&chunk);
				return 0;
}
