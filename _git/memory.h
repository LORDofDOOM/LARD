//////////////////////////////// memory.h ////////////////////////////
//
//
//
//
//

#ifndef MEMORY_H_
#define MEMORY_H_

void * safeMalloc (size_t size);

extern uint32 __heap_size;

#define malloc  malloc_not_allowed_use_safemalloc

#endif /* MEMORY_H_ */
