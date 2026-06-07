#include "common.h"
#include "memory.h"

void* reallocate(void *pointer, size_t oldCap, size_t newCap) {
				if (newCap == 0) {
								free(pointer);
								return NULL;
				}
				
				void* result = realloc(pointer, newCap);

				// In the offchance there isnt enough memory handle NULL return from realloc ;p
				if (result == NULL) exit(1);

				return result;
}
