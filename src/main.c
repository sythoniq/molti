#include "./common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
				Chunk chunk;
				initializeVM();
				initValueChunk(&chunk);
				int constant = addConstant(&chunk, 100);
				writeChunk(&chunk, OP_CONSTANT, 1);
				writeChunk(&chunk, constant, 1);

				constant = addConstant(&chunk, 50);
				writeChunk(&chunk, OP_CONSTANT, 1);
				writeChunk(&chunk, constant, 1);
				
				writeChunk(&chunk, OP_ADD, 1);

				constant = addConstant(&chunk, 50);
				writeChunk(&chunk, OP_CONSTANT, 1);
				writeChunk(&chunk, constant, 1);

				writeChunk(&chunk, OP_DIVIDE, 1);

				writeChunk(&chunk, OP_RETURN, 3);
				disassembleChunk(&chunk, "test chunk");
				interpret(&chunk);
				freeVM();
				freeValueChunk(&chunk);
				return 0;
}
