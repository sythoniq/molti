#ifndef molti_trace_h
#define molti_trace_h

#include "chunk.h"
	
void traceInit();
void traceClose();
void traceStep(Chunk *chunk, uint8_t *ip);
void traceAlloc(void* p, int type, size_t size);
void traceFree(void* p);

#endif
