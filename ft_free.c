#include "ft_malloc.h"

void    ft_free(void *ptr)
{
    t_block *block;

    if (!ptr)
        return ;
    pthread_mutex_lock(&g_malloc.mutex);
    block = (t_block *)ptr - 1;
    if (block->size >= 1024)
        munmap(block, sizeof(t_block) + block->size);
    else
        block->is_free = true;
    pthread_mutex_unlock(&g_malloc.mutex);
}

// bonus : Defragment” the freed memory