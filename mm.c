#include <stddef.h>

#define WSIZE 4             //WORD size
#define DSIZE 8             //Double word size
#define CHUNKSIZE (1<<12)   //Extend heap by this amount

#define MAX(a,b)        (((a) > (b))? (a) : (b))
//Pack size and allocated bit into a word
#define PACK(size,alloc)       ((size) | (alloc))

//Read and write a word at address p
#define GET(p)          (*(unsigned int*)(p))
#define PUT(p,val)      (*(unsigned int*)(p) = (val))

//Read the size and allocated fields from address p
#define GET_SIZE(p)     (GET(p) & (~0x7))
#define GET_ALLOC(p)    (GET(p) & 0x1)

//Given block ptr bp, compute address of its header and footer
#define HDRP(bp)        ((char*)(bp) - WSIZE)
#define FTRP(bp)        ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

//Given block ptr bp, compute the next and previous block address
#define NEXT_BP(bp)     ((char*)(bp) + GET_SIZE((char*)(bp) - WSIZE))
#define PREV_BP(bp)     ((char*)(bp) - GET_SIZE((char*)(bp) - DSIZE))

extern void *mem_sbrk(int incr);
static void *extend_heap(size_t size);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);
static void *heap_listp;

int mm_init() {

    if((heap_listp = mem_sbrk(4*WSIZE)) == (void*)-1)
       return -1;
    PUT(heap_listp, 0);                         //Aligment padding
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE,1));   //Prologue header
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE,1));   //Prologue footer
    PUT(heap_listp + (3*WSIZE), PACK(0,1));       //Epilogue header
    heap_listp += (2*WSIZE);

    //Extend the empty heap with a free block of CHUNKSIZE bytes
    if(extend_heap(CHUNKSIZE) == NULL)
        return -1;
    return 0;
}

void *mm_malloc(size_t size) {
    if(size == 0)
        return NULL;
    
    void *bp;
    //Adjust block size to include overhead and alignment reqs
    size_t asize = DSIZE * ((DSIZE + size + (DSIZE-1)) / DSIZE);

    //Search the free list for a fit
    if((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    //No fit found. Get more memory and place the block
    size_t extendsize = MAX(asize, CHUNKSIZE);
    if( (bp=extend_heap(extendsize)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    coalesce(bp);
}

static void *extend_heap(size_t size) {
    char *bp;
    size_t word = size / WSIZE;
    //Allocate an even number of words to maintain alignment
    if(word % 2 == 1)
        size += WSIZE;
    if((long)(bp = (char*)mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size,0));            //Current free block header
    PUT(FTRP(bp), PACK(size,0));            //Current free block footer
    PUT(HDRP(NEXT_BP(bp)), PACK(0,1));      //Move epilogue header to the end of heap

    //Coalesce if previous block is free
    return coalesce(bp);
}


static void *find_fit(size_t asize) {
    //First fit search
    void *bp;
    size_t size;
    for(bp = heap_listp; size = GET_SIZE(HDRP(bp)); bp = NEXT_BP(bp)) {
        if(!GET_ALLOC(HDRP(bp)) && (size >= asize))
            return bp;
    }
    return NULL; //Not fit
}

static void place(void *bp, size_t asize) {
    size_t size = GET_SIZE(HDRP(bp));
    if((size - asize) >= 2*DSIZE) { //Need to split
        PUT(HDRP(bp), PACK(asize,1));
        PUT(FTRP(bp), PACK(asize,1));
        bp = NEXT_BP(bp);
        PUT(HDRP(bp), PACK(size-asize,0));
        PUT(FTRP(bp), PACK(size-asize,0));
    }
    else {  //Allocate the whole free chunk
        PUT(HDRP(bp), PACK(size,1));
        PUT(FTRP(bp), PACK(size,1));
    }
}

static void *coalesce(void *bp) {
    int next_alloc = GET_ALLOC(HDRP(NEXT_BP(bp)));
    int prev_alloc = GET_ALLOC(FTRP(PREV_BP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(next_alloc && prev_alloc)
        return bp;
    
    else if(next_alloc && !prev_alloc) {
        size += GET_SIZE(FTRP(PREV_BP(bp)));
        PUT(HDRP(PREV_BP(bp)), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));
        bp = PREV_BP(bp);   //block ptr points to the previous block
    }
    
    else if(!next_alloc && prev_alloc) {
        size += GET_SIZE(HDRP(NEXT_BP(bp)));
        PUT(HDRP(bp), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));  //Attention! Size of block bp has been changed!
    }

    else {
        size += GET_SIZE(FTRP(PREV_BP(bp))) + GET_SIZE(HDRP(NEXT_BP(bp)));
        PUT(HDRP(PREV_BP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BP(bp)), PACK(size,0));
        bp = PREV_BP(bp);   //block ptr points to the previous block
    }
    
    return bp;
}
