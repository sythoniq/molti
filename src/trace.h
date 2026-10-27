#ifndef molti_trace_h
#define molti_trace_h
	
void traceInit();
void traceClose();
void traceStep(int op, int line);
void traceAlloc(void* p, int type, size_t size);
void traceFree(void* p);

#endif
