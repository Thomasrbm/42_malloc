// bad_malloc.c
#include <sys/mman.h>
#include <stddef.h>

void *bad_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    void *ptr = mmap(NULL, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1, 0);
    if (ptr == MAP_FAILED)
        return NULL;
    return ptr;
}

void bad_free(void *ptr, size_t size)
{
    if (!ptr)
        return;
    munmap(ptr, size);
}

int main(void)
{
    void *ptrs[1000000];

    // 1 million d allocations = 1 million de syscalls mmap
    for (int i = 0; i < 1000000; i++)
        ptrs[i] = bad_malloc(64);
    for (int i = 0; i < 1000000; i++)
        bad_free(ptrs[i], 64);
    return 0;
}