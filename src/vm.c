#include <stdio.h>
#include "vm.h"
#include "common.h"
#include "debug.h"

// Declaring a global vm variable to not have to pass a pointer to vm to each function.
VM vm;

// Helper function to reset the stack when needed.
static void resetStack() {
				vm.stackTop = vm.stack;
}

void initializeVM() {
				resetStack();
}

void freeVM() {

}

void pushValueToStack(Value value) {
				*vm.stackTop = value;
				vm.stackTop++;
}

Value popValueFromStack() {
				vm.stackTop--;
				return *vm.stackTop;
}

static InterpreterResult run() {
#define READ_BYTE() (*vm.ip++) // The ip points to the instruction that is about to be executed ( always point to the next instruction and not the one being currently executed). As such the macro increments ip when this function (run) is called within the interpret function.

#ifdef DEBUG_TRACE_EXECUTION
				printf("-------------------------------------------------------");
				for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
								printf("[");
								printValue(*slot);
								printf("]");
				}
				printf("\n");
				disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
				for (;;) {
								uint8_t instruction;
								switch (instruction = READ_BYTE()) {
												case OP_CONSTANT: {
																Value constant = READ_CONSTANT();
																pushValueToStack(constant);
																break;
												}
												case OP_RETURN: {
																printValue(popValueFromStack());
																printf("\n");
																return INTERPRET_OK;
												}
								}
				}

#undef READ_CONSTANT
#undef READ_BYTE
}

InterpreterResult interpret(Chunk *chunk) {
				vm.chunk = chunk;
				vm.ip = vm.chunk->code;
				return run();
}
