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

void push(Value value) {
				*vm.stackTop = value;
				vm.stackTop++;
}

Value pop() {
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
#define BINARY_OP(op) \
				do { \
								double b = pop(); \
								double a = pop(); \
								push(a op b); \
				} while (false)
				for (;;) {
								uint8_t instruction;
								switch (instruction = READ_BYTE()) {
												case OP_CONSTANT: {
																Value constant = READ_CONSTANT();
																push(constant);
																break;
												}
												case OP_ADD: BINARY_OP(+); break;	
												case OP_SUBTRACT: BINARY_OP(-);break;
												case OP_MULTIPLY: BINARY_OP(*);break;
												case OP_DIVIDE: BINARY_OP(/);break;
												case OP_NEGATE: push(-pop());break;
												case OP_RETURN: {
																printValue(pop());
																printf("\n");
																return INTERPRET_OK;
												}
								}
				}

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpreterResult interpret(Chunk *chunk) {
				vm.chunk = chunk;
				vm.ip = vm.chunk->code;
				return run();
}
