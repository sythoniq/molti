#ifndef MOLTI_VM_H
#define MOLTI_VM_H

#include "chunk.h"

typedef struct {
				Chunk* chunk;
				uint8_t* ip;
} VM;

typedef enum {
				INTERPRET_OK,
				INTERPRET_COMPILE_ERROR,
				INTERPRET_RUNTIME_ERROR
} InterpreterResult;

void initVM();
void freeVM();

InterpreterResult interpret(Chunk *chunk);
#endif
