#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "vm.h"
#include "chunk.h"

static FILE* out = NULL;
static long step = 0;

void traceInit() {
  const char* path = getenv("TRACE");
	if (!path) {
		out = fopen("trace.json", "w");
	}
  if (path) { 
		out = fopen(path, "w");
	}
}

void traceClose() {
	if (out) {
		fclose(out);
	}
}

static const char* opcodeName(uint8_t op) {
	switch (op) {
		case OP_CONSTANT: return "OP_CONSTANT";
		case OP_NIL: return "OP_NIL";
		case OP_TRUE: return "OP_TRUE";
		case OP_FALSE: return "OP_FALSE";
		case OP_POP: return "OP_POP";
		case OP_GET_LOCAL: return "OP_GET_LOCAL";
		case OP_SET_LOCAL: return "OP_SET_LOCAL";
		case OP_GET_GLOBAL: return "OP_GET_GLOBAL";
		case OP_DEFINE_GLOBAL: return "OP_DEFINE_GLOBAL";
		case OP_SET_GLOBAL: return "OP_SET_GLOBAL";
		case OP_EQUAL: return "OP_EQUAL";
		case OP_GREATER: return "OP_GREATER";
		case OP_LESS: return "OP_LESS";
		case OP_ADD: return "OP_ADD";
		case OP_SUBTRACT: return "OP_SUBTRACT";
		case OP_MULTIPLY: return "OP_MULTIPLY";
		case OP_DIVIDE: return "OP_DIVIDE";
		case OP_NOT: return "OP_NOT";
		case OP_NEGATE: return "OP_NEGATE";
		case OP_PRINT: return "OP_PRINT";
		case OP_JUMP: return "OP_JUMP";
		case OP_JUMP_IF_FALSE: return "OP_JUMP_IF_FALSE";
		case OP_LOOP: return "OP_LOOP";
		case OP_CALL: return "OP_CALL";
		case OP_RETURN: return "OP_RETURN";
		default: return "OP_UNKNOWN";
	}
}

static void jsonString(const char* s, int len) {
  fputc('"', out);
  for (int i = 0; i < len; i++) {
    unsigned char c = s[i];
    if (c == '"' || c == '\\') fprintf(out, "\\%c", c);
    else if (c < 0x20) fprintf(out, "\\u%04x", c);
    else fputc(c, out);
  }
  fputc('"', out);
}

static void jsonValue(Value v) {
	switch (v.type) {
		case VAL_NIL: 
			fprintf(out, "{\"type\":\"nil\",\"value\":null}"); 
			break;
		case VAL_BOOL: 
			fprintf(out, "{\"type\":\"bool\",\"value\":%s}", AS_BOOL(v) ? "true" : "false"); 
			break;
		case VAL_NUMBER: 
			fprintf(out, "{\"type\":\"num\",\"value\":%.17g}", AS_NUMBER(v)); 
			break;
		case VAL_OBJ:
			fprintf(out, "{\"type\":\"obj\",\"id\":\"%p\"", (void*)AS_OBJ(v));
			if (IS_STRING(v)) {
				fprintf(out, ",\"value\":");
				jsonString(AS_CSTRING(v), AS_STRING(v)->length);
			}
			fputc('}', out);
			break;
	}
}

static void writeDescription(uint8_t op, Chunk* chunk, uint8_t* ip) {
	fprintf(out, "\"description\":\"");
	switch (op) {
		case OP_CONSTANT:
			fprintf(out, "Push constant at index %d to stack", ip[1]);
			break;
		case OP_GET_LOCAL:
			fprintf(out, "Read local variable at stack slot %d", ip[1]);
			break;
		case OP_SET_LOCAL:
			fprintf(out, "Write top of stack to local variable at slot %d", ip[1]);
			break;
		case OP_GET_GLOBAL:
		case OP_DEFINE_GLOBAL:
		case OP_SET_GLOBAL: {
			uint8_t constant = ip[1];
			Value name = chunk->constants.values[constant];
			fprintf(out, "%s global variable ", 
					 op == OP_DEFINE_GLOBAL ? "Define" : (op == OP_SET_GLOBAL ? "Set" : "Get"));
			if (IS_STRING(name)) jsonString(AS_CSTRING(name), AS_STRING(name)->length);
			else fprintf(out, "\"unknown\"");
			break;
		}
		case OP_ADD: fprintf(out, "Add top two stack values"); break;
		case OP_SUBTRACT: fprintf(out, "Subtract top two stack values"); break;
		case OP_MULTIPLY: fprintf(out, "Multiply top two stack values"); break;
		case OP_DIVIDE: fprintf(out, "Divide top two stack values"); break;
		case OP_POP: fprintf(out, "Pop and discard top value from stack"); break;
		case OP_PRINT: fprintf(out, "Print top of stack to console"); break;
		case OP_RETURN: fprintf(out, "Return from current function"); break;
		default: fprintf(out, "Execute %s", opcodeName(op)); break;
	}
	fprintf(out, "\"");
}

void traceStep(Chunk* chunk, uint8_t* ip) {
	if (!out) return;

	uint8_t op = *ip;
	int offset = (int)(ip - chunk->code);
	int line = chunk->lines[offset];

	fprintf(out, "{\"step\":%ld,\"action\":\"EXECUTE\",\"instruction\":\"%s\",\"line\":%d,", 
				 step++, opcodeName(op), line);

	writeDescription(op, chunk, ip);

	fprintf(out, ",\"stack\":[");
	for (Value* s = vm.stack; s < vm.stackTop; s++) {
		if (s != vm.stack) fputc(',', out);
		jsonValue(*s);
	}
	fprintf(out, "],\"frames\":[");

	for (int i = 0; i < vm.frameCount; i++) {
		if (i) fputc(',', out);
		CallFrame* f = &vm.frames[i];
		int currentLine = f->function->chunk.lines[f->ip - f->function->chunk.code];
		fprintf(out, "{\"function\":\"%s\",\"line\":%d,\"stackBase\":%ld}",
					f->function->name ? f->function->name->chars : "<script>",
					currentLine,
					(long)(f->slots - vm.stack));
	}

	fprintf(out, "]}\n");
	fflush(out); // ensures frontend can read the file in real-time
}

void traceAlloc(void* p, int type, size_t size) {
	if (!out) return;
	// TODO: Map type to a string

	const char* typeStr = "object";
	fprintf(out, "{\"step\":%ld,\"action\":\"ALLOC\",\"id\":\"%p\",\"type\":\"%s\",\"size\":%zu}\n", 
				 step, p, typeStr, size);
	fflush(out);
}

void traceFree(void* p) {
	if (!out) return;
	fprintf(out, "{\"step\":%ld,\"action\":\"FREE\",\"id\":\"%p\"}\n", step, p);
	fflush(out);
}
