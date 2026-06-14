#include "./common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
				Chunk chunk;
				initValueChunk(&chunk);
				writeConstant(&chunk, 100, 1);
				writeChunk(&chunk, OP_RETURN, 2);
				disassembleChunk(&chunk, "test chunk");
				freeValueChunk(&chunk);
				return 0;
}
