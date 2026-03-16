#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdlib.h>

typedef struct s_block
{
    size_t          size;
    bool            is_free;
    struct s_block  *next;
}   t_block;

typedef struct s_zone
{
    size_t          total;
    size_t          used;
    struct s_zone   *next;  // vers zone suivante de * 100 (128 000)
    t_block         *blocks; // vers debut de la liste de block de 128 ou 1024 etc
}   t_zone;

typedef struct s_malloc
{
    t_zone          *tiny;
    t_zone          *small;
    t_zone          *large;
    pthread_mutex_t mutex;
}   t_malloc;

extern t_malloc g_malloc;

void    ft_free(void *ptr);
