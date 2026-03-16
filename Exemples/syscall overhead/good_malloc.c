#include <sys/mman.h>
#include <stddef.h>
#include <stdlib.h>

int main(void)
{
    void *ptrs[1000000];

    for (int i = 0; i < 1000000; i++)
        ptrs[i] = malloc(64);
    for (int i = 0; i < 1000000; i++)
        free(ptrs[i]);
    return 0;
}


// cc bad_malloc.c  -o bad_malloc
// cc good_malloc.c -o good_malloc




// ctrl + c le bad malloc sinon cest ultra long

// strace -c ./bad_malloc  2>&1
// strace -c ./good_malloc 2>&1

// time ./bad_malloc
// time ./good_malloc