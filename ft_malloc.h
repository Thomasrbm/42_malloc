#pragma once



typedef t_tiny
{
    void *addresse;
    void *next;
    size_t space_left;
    bool is_alloc;
} t_tiny

typedef t_small
{
    void *addresse;
    void *next;
    size_t space_left;
    bool is_alloc;
} t_small

typedef t_large
{
    void *addresse;
    void *next;
    size_t space_left;
    bool is_alloc;
} t_large



typedef global t_malloc
{
    **t_tiny tiny;
    **t_small small;
    **t_large large;
} t_malloc

