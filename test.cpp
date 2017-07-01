#include <stdlib.h>
#include <stdio.h>

extern void mem_init(void);
extern int mm_init(void);
extern void *mm_malloc(size_t size);
extern void mm_free(void *ptr);

int main() {
    char *a, *b, *c, *d, *e;
    
    printf("now mem_init ...");
    getchar();
    mem_init();   
    if(mm_init() == -1) {
        fprintf(stderr, "ERROR: mm_init failed\n");
        return -1;
    }
    printf("after mm_init\n");
    
    printf("\nnow first mm_malloc(8) ...");
    getchar();
    if((a=(char*)mm_malloc(8)) == NULL) {
        fprintf(stderr, "ERROR: mm_malloc failed\n");
        return -1;
    } 
    printf("the first malloc addr - a: %p\n",a);
    
    printf("\nnow second mm_malloc(40) ...");
    getchar();
    if((b=(char*)mm_malloc(40)) == NULL) {
        fprintf(stderr, "ERROR: mm_malloc failed\n");
        return -1;
    } 
    printf("the second malloc addr - b: %p\n",b);
    
    printf("\nnow mm_free b ...");
    getchar();
    mm_free(b);
    printf("after free b\n");
    
    printf("\nnow third mm_malloc(7) ...\n");
    printf("should place in b");
    getchar();
    if((c=(char*)mm_malloc(7)) == NULL) {
        fprintf(stderr, "ERROR: mm_malloc failed\n");
        return -1;
    } 
    printf("the third malloc addr - c: %p\n",c);
    
    printf("\nnow mm_free a ...");
    getchar();
    mm_free(a);
    printf("after free a\n");
    
    printf("\nnow forth mm_malloc(9) ...\n");
    printf("should place after c, not at a (9>8)");
    getchar();
    if((d=(char*)mm_malloc(9)) == NULL) {
        fprintf(stderr, "ERROR: mm_malloc failed\n");
        return -1;
    } 
    printf("the forth malloc addr - d: %p\n",d);

    printf("\nnow mm_free c ...\n");
    printf("should coalesce with a");
    getchar();
    mm_free(c);
    printf("after free c\n");
    
    printf("\nnow fifth mm_malloc(14) ...\n");
    printf("should place at a");
    getchar();
    if((e=(char*)mm_malloc(14)) == NULL) {
        fprintf(stderr, "ERROR: mm_malloc failed\n");
        return -1;
    } 
    printf("the fifth malloc addr - e: %p\n",e);

    printf("\nnow mm_free d ...");
    getchar();
    mm_free(d);
    printf("after free d\n");
    
    printf("\nnow mm_free e ...");
    getchar();
    mm_free(e);
    printf("after free e\n");
    
    return 0;
}
