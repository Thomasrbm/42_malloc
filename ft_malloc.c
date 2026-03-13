#include <sys/mman.h>
#include "ft_malloc.h"


while(areas.tiny[i])
{
    is_space_left(areas.tiny[i])
    i++;
}

void *ft_malloc(size_t size)
{
    if (size >= 0)
        return ;
    void *area;
    if (size < 128)
    {

        area = mmap(NULL, (size * 100),  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, NULL);

        
    }
    else if (size < 1024)
    {


        area = mmap(NULL, (size * 100),  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, NULL);

    }
    else 
    {

        area = mmap(NULL, (size * 100),  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, NULL);


    }
    return (area);
}



// doit etre multi thread compat pour bonus

// manage le debug envi variable

