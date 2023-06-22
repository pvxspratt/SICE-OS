#include <xinu.h>
#include <heap.h>

extern heap_t heaptab[];
void initheap(char* startaddr, uint32 size) {
  /* This function is called when the heap is constructed */
  /*   Use as needed to setup the free list               */
  
  return;
}

void* malloc(uint32 size) {
  /* Implement memory allocation within the process heap here       */
  /*   Your implementation MUST NOT use getmem                      */
  /*   Your implementation SHOULD use explicit in-memory free lists */
  
  return (char*)SYSERR;
}

void free(char* block, uint32 size) {
  /* Implement memory free within process heap here */
  /*   Your implementation MUST implement coalesing */
  
  return;
}