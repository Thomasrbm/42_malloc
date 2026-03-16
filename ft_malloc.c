#include "ft_malloc.h"

t_malloc    g_malloc = {NULL, NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

t_zone  *find_free_zone(t_zone *zone, size_t size)
{
    while (zone)
    {
        if (zone->total - zone->used >= size)
            return (zone);
        zone = zone->next;
    }
    return (NULL);
}

t_zone  *ft_new_zone(t_zone **list, size_t zone_size)   // pointe deja sur la zone null  soit la next d une pleine, soit la premiere a nulle (voir find free zone)
{
    t_zone  *zone;

    zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANON, -1, 0);
    if (zone == MAP_FAILED)
        return (NULL);
    zone->total = zone_size - sizeof(t_zone);   // des le debut sort la place de la struct garde la raw zone
    zone->used = 0;
    zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));   // met le premier block a partir d apres la taille de la structure.     => pointe sur debut la ou seront block MAIS PAS DE BLOCK POUR L INSTANT 
    zone->next = *list;   // on push front et non back la new zone, sera plus opti pour check les free zone au lieu de demmarer par les vielles qu on sait deja remplies.
    *list = zone;
    return (zone);
}


// sans block splitting.  un block = un malloc.  un block va pas partager plusieur malloc si possible => faire simple ici.
void    *push_in_zone(t_zone *zone, size_t size)
{
    t_block *block;

    block = zone->blocks;
    while (block) // cherche block is free.
    {
        if (block->is_free)
        {
            block->is_free = false;
            return ((void *)(block + 1));
        }
        block = block->next;
    }

    block = (t_block *)((char *)zone->blocks + zone->used); // start juste apres le block de memoire struct + ceux deja alouee.
    block->size = size;
    block->is_free = false;
    block->next = NULL;
    zone->used += sizeof(t_block) + size;
    return ((void *)(block + 1));
}

void    *ft_malloc(size_t size)
{
    t_zone  *zone;
    void    *area;

    if (size == 0)
        return (NULL);
    pthread_mutex_lock(&g_malloc.mutex);
    if (size < 128)
    {
        zone = find_free_zone(g_malloc.tiny, size);
        if (!zone)
            zone = ft_new_zone(&g_malloc.tiny, sizeof(t_zone) + 128 * 100);  // alloue la zone de la strucutre  +  la vrai zone memoire
        area = push_in_zone(zone, 128);
    }
    else if (size < 1024)
    {
        zone = find_free_zone(g_malloc.small, size);
        if (!zone)
            zone = ft_new_zone(&g_malloc.small, sizeof(t_zone) + 1024 * 100);
        area = push_in_zone(zone, 1024);
    }
    else
    {
        zone = find_free_zone(g_malloc.large, size);
        if (!zone)
            zone = ft_new_zone(&g_malloc.large, sizeof(t_zone) + size);
        area = push_in_zone(zone, size);
    }
    pthread_mutex_unlock(&g_malloc.mutex);
    return (area);
}

#include "ft_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char    *str1;
    char    *str2;
    int     *tab;
    char    *big;
    int     i;

    printf("%-25s %-20s %-20s\n", "test", "malloc", "ft_malloc");
    printf("--------------------------------------------------------------------\n");

    // test tiny
    str1 = ft_malloc(50);
    str1[0] = 'H'; str1[1] = 'i'; str1[2] = '\0';
    char *ref1 = malloc(50);
    ref1[0] = 'H'; ref1[1] = 'i'; ref1[2] = '\0';
    printf("%-25s %-20s %-20s\n", "str1 (50)", ref1, str1);

    // test tiny 2
    str2 = ft_malloc(80);
    str2[0] = '4'; str2[1] = '2'; str2[2] = '\0';
    char *ref2 = malloc(80);
    ref2[0] = '4'; ref2[1] = '2'; ref2[2] = '\0';
    printf("%-25s %-20s %-20s\n", "str2 (80)", ref2, str2);

    // test small
    tab = ft_malloc(sizeof(int) * 10);
    int *reftab = malloc(sizeof(int) * 10);
    i = 0;
    while (i < 10)
    {
        tab[i] = i * 2;
        reftab[i] = i * 2;
        i++;
    }
    printf("%-25s %-20d %-20d\n", "tab[0] (small)", reftab[0], tab[0]);
    printf("%-25s %-20d %-20d\n", "tab[5] (small)", reftab[5], tab[5]);
    printf("%-25s %-20d %-20d\n", "tab[9] (small)", reftab[9], tab[9]);

    // test large
    big = ft_malloc(2048);
    big[0] = 'L'; big[1] = '\0';
    char *refbig = malloc(2048);
    refbig[0] = 'L'; refbig[1] = '\0';
    printf("%-25s %-20s %-20s\n", "big (2048)", refbig, big);

    // test size 0
    void *null_ft = ft_malloc(0);
    void *null_real = malloc(0);
    printf("%-25s %-20p %-20p\n", "malloc(0)", null_real, null_ft);

    printf("--------------------------------------------------------------------\n");

    // test free + réutilisation
    void *addr_before = (void *)str1;
    ft_free(str1);
    free(ref1);
    str1 = ft_malloc(50);
    ref1 = malloc(50);
    str1[0] = 'R'; str1[1] = 'e'; str1[2] = 'u'; str1[3] = '\0';
    ref1[0] = 'R'; ref1[1] = 'e'; ref1[2] = 'u'; ref1[3] = '\0';
    printf("%-25s %-20s %-20s\n", "str1 apres free", ref1, str1);
    printf("%-25s %-20s %-20s\n", "meme adresse ?", "N/A",
        addr_before == (void *)str1 ? "yes" : "no");

    // test free NULL
    ft_free(NULL);
    free(NULL);
    printf("%-25s %-20s %-20s\n", "free(NULL)", "no crash", "no crash");

    // test free large
    ft_free(big);
    free(refbig);
    big = ft_malloc(2048);
    refbig = malloc(2048);
    big[0] = 'N'; big[1] = '\0';
    refbig[0] = 'N'; refbig[1] = '\0';
    printf("%-25s %-20s %-20s\n", "big apres free", refbig, big);

    // test memset apres malloc
    char *mem_ft = ft_malloc(100);
    char *mem_real = malloc(100);
    memset(mem_ft, 'A', 99);
    memset(mem_real, 'A', 99);
    mem_ft[99] = '\0';
    mem_real[99] = '\0';
    printf("%-25s %-20s %-20s\n", "memset 100 x 'A'",
        memcmp(mem_real, mem_ft, 100) == 0 ? "ok" : "diff",
        memcmp(mem_real, mem_ft, 100) == 0 ? "ok" : "diff");

    free(ref2);
    free(reftab);
    ft_free(str2);
    ft_free(tab);
    ft_free(str1);
    ft_free(mem_ft);
    free(mem_real);
    free(null_real);

    return (0);
}