#include <stdlib.h>
#include <stdio.h>

#define MAX_HEAP (1<<17)    //0x20000

static char *mem_heap;  //start of the whole heap memory
static char *mem_brk;   //end of allocated heap memory + 1
static char *mem_max_addr;  //max legal heap addr + 1

void mem_init() {
   mem_heap = (char*)malloc(MAX_HEAP); 
   mem_brk = mem_heap;
   mem_max_addr = mem_heap+MAX_HEAP;
}

void *mem_sbrk(int incr) {
    char *old_brk = mem_brk;
    if(incr<0 || mem_brk+incr>mem_max_addr) {
        fprintf(stderr, "ERROR: mem_sbrk failed.\n");
        return (void*)-1;
    }
    mem_brk += incr;
    return (void*)old_brk;
}

