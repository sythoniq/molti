#ifndef MOLTI_MEMORY_H
#define MOLTI_MEMORY_H

#include "common.h"

// This macro grow capacity by checking if the current capacity is less than x (to save memory for initialization from 0) then doubles the capacity.

#define GROW_CAPACITY(capacity) \
				((capacity) < 12 ? 12 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCap, newCap) \
			(type*)reallocate(pointer, sizeof(type) * (oldCap), \
										 sizeof(type) * (newCap))	

void* reallocate(void* pointer, size_t oldCap, size_t newCap);


#define FREE_ARRAY(type, pointer, cap) \
				reallocate(pointer, sizeof(type) * cap, 0)

#endif
