#include <sys/mman.h>
#include "ft_malloc.h"


while(areas.tiny[i])
{
    is_space_left(areas.tiny[i])
    i++;
}

void *ft_malloc(size_t size)
{
    if (size <= 0)
        return ;
    void *area;

    // lock avec mutex 
    if (size < 128)
    {

        // rediriger vers zone deja allouer.



        

        // reserver grosse zone direct le max (x 100)

        // sous condition , reserver que si y a pas de zones libres
        // normaliser par le multiple de 128
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


    // unlock mutex 
    return (area);
}



// doit etre multi thread compat pour bonus

// manage le debug envi variable

