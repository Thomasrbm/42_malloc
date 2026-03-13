#include "ft_malloc.h"



void *realloc(void *ptr, size_t size)
{


    mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, NULL);
    munmap(void addr[.length], size_t length);

}