// test_maps.c
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    // Allocation 1 : petite, glibc va utiliser brk (heap classique)
    char *petit = malloc(100);
    petit[0] = 42;

    // Allocation 2 : grande, glibc va utiliser mmap anonymous
    char *grand = malloc(200000);
    grand[0] = 99;

    // Allocation 3 : mmap anonymous direct (sans malloc)
    char *direct = mmap(NULL, 8192,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1, 0);
    direct[0] = 1;

    int fd = open("/etc/hostname", O_RDONLY);
    char *fichier = mmap(NULL, 4096,
                         PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    printf("PID : %d\n", getpid());
    printf("ptr petit  (heap/brk) : %p\n", (void*)petit);
    printf("ptr grand  (mmap anon): %p\n", (void*)grand);
    printf("ptr direct (mmap anon): %p\n", (void*)direct);
    printf("ptr fichier (mmap file): %p\n", (void*)fichier);
    printf("\nAppuie sur Entrée pour quitter...\n");
    fflush(stdout);

    getchar();

    free(petit);
    free(grand);
    munmap(direct, 8192);
    munmap(fichier, 4096);
    return 0;
}


// cat /proc/[mettre PID]/maps      => carte de la memoire virtuelle d un processus. 


// chaque ligne =  1 VMA 


// 564684bc8000-564684bc9000 r--p 00000000   a.out   ← en-tête ELF (métadonnées)
// 564684bc9000-564684bca000 r-xp 00001000   a.out   ← .text  = ton code compilé
// 564684bca000-564684bcb000 r--p 00002000   a.out   ← .rodata = tes strings const
// 564684bcb000-564684bcc000 r--p 00002000   a.out   ← .rodata (suite, relocation)
// 564684bcc000-564684bcd000 rw-p 00003000   a.out   ← .data/.bss = tes variables globales


// 772a51405000-772a51412000 rw-p 00000000 00:00 0     => les sans noms sont les mallocs 
// 772a51460000-772a51494000 rw-p 00000000 00:00 0  



// 7813a7724000 - 7813a7728000  : 7728000 - 7724000 = 0x4000 = 16384 octets = 4 pages de 4096 octets.    = espace pris 