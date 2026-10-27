#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "vm.h"

static FILE* out = NULL;
static long step = 0;

void traceInit() {
  const char* path = getenv("TRACE");
  if (path) { 
		out = fopen(path, "w");
	}
}

void traceClose() {
	if (out) {
		fclose(out);
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
    case VAL_NIL:    fprintf(out, "{\"t\":\"nil\"}"); break;
    case VAL_BOOL:   fprintf(out, "{\"t\":\"bool\",\"v\":%s}",
                             AS_BOOL(v) ? "true" : "false"); break;
    case VAL_NUMBER: fprintf(out, "{\"t\":\"num\",\"v\":%.17g}",
                             AS_NUMBER(v)); break;
    case VAL_OBJ:
      fprintf(out, "{\"t\":\"obj\",\"id\":\"%p\"", (void*)AS_OBJ(v));
      if (IS_STRING(v)) {
        fprintf(out, ",\"v\":");
        jsonString(AS_CSTRING(v), AS_STRING(v)->length);
      }
      fputc('}', out);
      break;
  }
}

void traceStep(int op, int line) {
  if (!out) return;
  fprintf(out, "{\"ev\":\"step\",\"n\":%ld,\"op\":%d,\"line\":%d,\"stack\":[",
          step++, op, line);
  for (Value* s = vm.stack; s < vm.stackTop; s++) {
    if (s != vm.stack) fputc(',', out);
    jsonValue(*s);
  }
  fprintf(out, "],\"frames\":[");
  for (int i = 0; i < vm.frameCount; i++) {
    if (i) fputc(',', out);
    CallFrame* f = &vm.frames[i];
    fprintf(out, "{\"fn\":\"%s\",\"base\":%ld}",
            f->function->name ? f->function->name->chars : "<script>",
            (long)(f->slots - vm.stack));
  }
  fprintf(out, "]}\n");
}

void traceAlloc(void* p, int type, size_t size) {
  if (out) fprintf(out,
      "{\"ev\":\"alloc\",\"n\":%ld,\"id\":\"%p\",\"type\":%d,\"size\":%zu}\n",
      step, p, type, size);
}

void traceFree(void* p) {
  if (out) fprintf(out, "{\"ev\":\"free\",\"n\":%ld,\"id\":\"%p\"}\n", step, p);
}
