// So a chunk is a sequence of bytecode

#ifndef molti_chunk_h
#define molti_chunk_h

#include "common.h"

/*
- Using typedef to work as an alias and remove the need to repeat enum on every call of a enum variable/instance. 
- Each value within the enum is an instruction = operation code so OP_RETURN is a return instruction ie, return from this function.
*/
typedef enum {
				OP_RETURN
} OpCode;

#endif
