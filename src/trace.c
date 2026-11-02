#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "vm.h"
#include "chunk.h"

static FILE* out = NULL;
static long step = 0;
static int pending = 0;
static long pendingStep = 0;
static int shuttingDown = 0;

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

static void writeStack(void) {
	fprintf(out, "\"stack\":[");
	for (Value* s = vm.stack; s < vm.stackTop; s++) {
		if (s != vm.stack) fputc(',', out);
		jsonValue(*s);
	}
	fputc(']', out);
}

static void writeFrames(void) {
	fprintf(out, "\"frames\":[");
	for (int i = 0; i < vm.frameCount; i++) {
		if (i) fputc(',', out);
		CallFrame* f = &vm.frames[i];
		int currentLine = f->function->chunk.lines[f->ip - f->function->chunk.code];
		fprintf(out, "{\"function\":\"%s\",\"line\":%d,\"stackBase\":%ld}",
					f->function->name ? f->function->name->chars : "<script>",
					currentLine, (long)(f->slots - vm.stack));
	}
	fputc(']', out);
}

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

static void traceResult(void) {
	if (!out || !pending) return;
	fprintf(out, "{\"step\":%ld,\"action\":\"RESULT\",", pendingStep);
	writeStack(); fputc(',', out); writeFrames();
	fprintf(out, "}\n");
	fflush(out);
	pending = 0;
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

static void fmtValue(char* buf, size_t n, Value v) {
	switch (v.type) {
		case VAL_NIL:    snprintf(buf, n, "nil"); break;
		case VAL_BOOL:   snprintf(buf, n, "%s", AS_BOOL(v) ? "true" : "false"); break;
		case VAL_NUMBER: snprintf(buf, n, "%g", AS_NUMBER(v)); break;
		case VAL_OBJ:
			if (IS_STRING(v)) snprintf(buf, n, "\"%s\"", AS_CSTRING(v));
			else if (IS_FUNCTION(v)) {
				ObjFunction* f = AS_FUNCTION(v);
				snprintf(buf, n, "<fn %s>", f->name ? f->name->chars : "script");
			} else snprintf(buf, n, "<object %p>", (void*)AS_OBJ(v));
			break;
	}
}

static void writeDescription(uint8_t op, Chunk* chunk, uint8_t* ip) {
	char d[256], a[96], b[96], r[96];
	CallFrame* frame = &vm.frames[vm.frameCount - 1];
	int depth = (int)(vm.stackTop - vm.stack);
	Value top    = depth >= 1 ? vm.stackTop[-1] : NIL_VAL;
	Value second = depth >= 2 ? vm.stackTop[-2] : NIL_VAL;
	switch (op) {
		case OP_CONSTANT:
			fmtValue(a, sizeof a, chunk->constants.values[ip[1]]);
			snprintf(d, sizeof d, "Push constant %s (constants[%d]) onto the stack", a, ip[1]);
			break;
		case OP_NIL:   snprintf(d, sizeof d, "Push nil onto the stack"); break;
		case OP_TRUE:  snprintf(d, sizeof d, "Push true onto the stack"); break;
		case OP_FALSE: snprintf(d, sizeof d, "Push false onto the stack"); break;
		case OP_POP:
			fmtValue(a, sizeof a, top);
			snprintf(d, sizeof d, "Pop and discard %s", a);
			break;
		case OP_GET_LOCAL: {
			int stackIndex = (int)(frame->slots - vm.stack) + ip[1];
			fmtValue(a, sizeof a, frame->slots[ip[1]]);
			snprintf(d, sizeof d, "Push local in slot %d (stack index %d, value %s) onto the stack",
						ip[1], stackIndex, a);
			break;
		}
		case OP_SET_LOCAL: {
			int stackIndex = (int)(frame->slots - vm.stack) + ip[1];
			fmtValue(a, sizeof a, top);
			snprintf(d, sizeof d, "Store %s into local slot %d (stack index %d)", a, ip[1], stackIndex);
			break;
		}
		case OP_GET_GLOBAL: {
			ObjString* name = AS_STRING(chunk->constants.values[ip[1]]);
			Value v;
			if (tableGet(&vm.globals, name, &v)) fmtValue(a, sizeof a, v);
			else snprintf(a, sizeof a, "<undefined>");
			snprintf(d, sizeof d, "Push global '%s' (value %s) onto the stack", name->chars, a);
			break;
		}
		case OP_DEFINE_GLOBAL:
		case OP_SET_GLOBAL: {
			ObjString* name = AS_STRING(chunk->constants.values[ip[1]]);
			fmtValue(a, sizeof a, top);
			snprintf(d, sizeof d, "%s global '%s' = %s",
						op == OP_DEFINE_GLOBAL ? "Define" : "Assign", name->chars, a);
			break;
		}
		case OP_ADD: case OP_SUBTRACT: case OP_MULTIPLY: case OP_DIVIDE: {
			const char* sym = op==OP_ADD?"+":op==OP_SUBTRACT?"-":op==OP_MULTIPLY?"*":"/";
			fmtValue(a, sizeof a, second);
			fmtValue(b, sizeof b, top);
			if (IS_NUMBER(second) && IS_NUMBER(top)) {
				double x = AS_NUMBER(second), y = AS_NUMBER(top);
				double res = op==OP_ADD ? x+y : op==OP_SUBTRACT ? x-y : op==OP_MULTIPLY ? x*y : x/y;
				snprintf(r, sizeof r, "%g", res);
				snprintf(d, sizeof d, "Pop %s and %s, compute %s %s %s = %s, push %s", a, b, a, sym, b, r, r);
			} else {
				snprintf(d, sizeof d, "Pop %s and %s, push %s %s %s", a, b, a, sym, b);
			}
			break;
		}
		case OP_EQUAL: case OP_GREATER: case OP_LESS: {
			const char* sym = op==OP_EQUAL?"==":op==OP_GREATER?">":"<";
			fmtValue(a, sizeof a, second); fmtValue(b, sizeof b, top);
			snprintf(d, sizeof d, "Pop %s and %s, push result of %s %s %s", a, b, a, sym, b);
			break;
		}
		case OP_NOT:    fmtValue(a, sizeof a, top); snprintf(d, sizeof d, "Replace %s with its logical negation", a); break;
		case OP_NEGATE: fmtValue(a, sizeof a, top); snprintf(d, sizeof d, "Replace %s with its numeric negation", a); break;
		case OP_PRINT:  fmtValue(a, sizeof a, top); snprintf(d, sizeof d, "Pop %s and print it", a); break;
		case OP_JUMP: {
			int off = (ip[1] << 8) | ip[2];
			snprintf(d, sizeof d, "Jump forward %d bytes", off); break;
		}
		case OP_JUMP_IF_FALSE: {
			int off = (ip[1] << 8) | ip[2];
			fmtValue(a, sizeof a, top);
			snprintf(d, sizeof d, "Condition is %s: %s (offset %d)", a,
						(IS_NIL(top) || (IS_BOOL(top) && !AS_BOOL(top))) ? "falsey, jump" : "truthy, continue", off);
			break;
		}
		case OP_LOOP: {
			int off = (ip[1] << 8) | ip[2];
			snprintf(d, sizeof d, "Jump back %d bytes (loop)", off); break;
		}
		case OP_CALL: {
			int argc = ip[1];
			fmtValue(a, sizeof a, vm.stackTop[-1 - argc]);
			snprintf(d, sizeof d, "Call %s with %d argument(s)", a, argc); break;
		}
		case OP_RETURN:
			fmtValue(a, sizeof a, top);
			snprintf(d, sizeof d, "Return %s from current function", a); break;
		default:
			snprintf(d, sizeof d, "Execute %s", opcodeName(op));
	}

	fputs("\"description\":", out);
	jsonString(d, (int)strlen(d));        // escapes the inner quotes properly
}


void traceStep(Chunk* chunk, uint8_t* ip) {
	if (!out) return;

	traceResult();

	uint8_t op = *ip;
	int offset = (int)(ip - chunk->code);
	int line = chunk->lines[offset];

	fprintf(out, "{\"step\":%ld,\"action\":\"EXECUTE\",\"instruction\":\"%s\",\"line\":%d,",
				 step++, opcodeName(op), line);

	writeDescription(op, chunk, ip);

	fputc(',', out);
	writeStack();
	fputc(',', out);
	writeFrames();
	fprintf(out, "}\n");
	fflush(out);

	pending = 1;
	pendingStep = step - 1;
}


void traceFinish(void) {      // called once when the program has finished running
	traceResult();
	shuttingDown = 1;
}

void traceAlloc(void* p, int type, size_t size) {
	if (!out) return;

	const char* typeStr = "object";
	switch(type) {
		case OBJ_STRING: typeStr="string"; break;
		case OBJ_FUNCTION: typeStr="function"; break;
		default: break;
	}
	fprintf(out, "{\"step\":%ld,\"action\":\"ALLOC\",\"id\":\"%p\",\"type\":\"%s\",\"size\":%zu}\n", 
				 step, p, typeStr, size);
	fflush(out);
}

void traceFree(void* p) {
	if (!out) return;
	fprintf(out, "{\"step\":%ld,\"action\":\"FREE\",\"id\":\"%p\",\"reason\":\"%s\"}\n",
				 step, p, shuttingDown ? "shutdown" : "gc");
	fflush(out);
}
