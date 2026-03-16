// observe_malloc.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

// Lit et affiche /proc/self/maps filtré sur les lignes anonymes + heap + stack
void affiche_maps(const char *moment)
{
    char ligne[512];
    printf("\n=== MAPS : %s ===\n", moment);
    FILE *f = fopen("/proc/self/maps", "r");
    while (fgets(ligne, sizeof(ligne), f))
    {
        // Affiche heap, stack, et anonymous (device 00:00 sans nom)
        if (strstr(ligne, "[heap]") || strstr(ligne, "[stack]")
            || (strstr(ligne, "00:00") && ligne[strlen(ligne)-2] == '0'))
            printf("  %s", ligne);
    }
    fclose(f);
}

// Lit RSS et VmSize depuis /proc/self/status
void affiche_ram(const char *moment)
{
    char ligne[256];
    printf("--- RAM : %s ---\n", moment);
    FILE *f = fopen("/proc/self/status", "r");
    while (fgets(ligne, sizeof(ligne), f))
        if (strncmp(ligne, "VmSize", 6) == 0 || strncmp(ligne, "VmRSS", 5) == 0)
            printf("  %s", ligne);
    fclose(f);
}

// Lit le RSS d'un VMA précis dans /proc/self/smaps
void affiche_rss_vma(void *ptr, const char *label)
{
    char ligne[256];
    char cible[32];
    // Forme "adresse-" pour trouver la bonne section dans smaps
    snprintf(cible, sizeof(cible), "%lx-", (unsigned long)ptr & ~0xFFFUL);
    FILE *f = fopen("/proc/self/smaps", "r");
    int dans_section = 0;
    while (fgets(ligne, sizeof(ligne), f))
    {
        if (strstr(ligne, cible))
            dans_section = 1;
        if (dans_section && strncmp(ligne, "Rss:", 4) == 0)
        {
            printf("  RSS du VMA %-20s : %s", label, ligne);
            break;
        }
        // Nouvelle section smaps = ligne avec '-' dans les 20 premiers chars
        if (dans_section && strchr(ligne, '-') && ligne[0] != ' ' && ligne != NULL)
            if (strstr(ligne, cible) == NULL)
                dans_section = 0;
    }
    fclose(f);
}

int main(void)
{
    printf("PID : %d\n", getpid());
    printf("Seuil mmap glibc : %zu octets\n", (size_t)128 * 1024);

    // =========================================================
    // ETAT INITIAL
    // =========================================================
    affiche_maps("initial");
    affiche_ram("initial");

    printf("\n\n╔══════════════════════════════════════╗");
    printf("\n║  PARTIE 1 : malloc(200000) → mmap    ║");
    printf("\n╚══════════════════════════════════════╝\n");

    // MALLOC GRAND : glibc va appeler mmap anonymous
    printf("\n[A] Avant malloc(200000) — appuie sur Entrée\n");
    getchar();
    affiche_maps("avant malloc grand");
    affiche_ram("avant malloc grand");

    char *grand = malloc(200000);

    printf("\n[B] malloc(200000) fait — VMA créé, ZERO RAM allouée\n");
    affiche_maps("après malloc(200000) — VMA existe, pas de RAM");
    affiche_ram("après malloc(200000)");
    affiche_rss_vma(grand, "grand (avant accès)");

    printf("\n[C] On va écrire grand[0] = 1 — appuie sur Entrée\n");
    getchar();
    grand[0] = 1;  // page fault → 1 frame allouée
    printf("[C] grand[0] écrit\n");
    affiche_ram("après grand[0]=1 (1 page fault)");
    affiche_rss_vma(grand, "grand (après page 0)");

    printf("\n[D] On écrit sur toutes les pages (200000 octets)\n");
    memset(grand, 0xFF, 200000);  // 49 page faults
    affiche_ram("après memset(200000) — 49 pages faultées");
    affiche_rss_vma(grand, "grand (toutes pages)");

    printf("\n[E] free(grand) — appuie sur Entrée\n");
    getchar();
    free(grand);
    printf("[E] free fait\n");
    affiche_maps("après free(grand) — VMA détruit");
    affiche_ram("après free(grand)");

    printf("\n\n╔══════════════════════════════════════╗");
    printf("\n║  PARTIE 2 : malloc(100) → brk        ║");
    printf("\n╚══════════════════════════════════════╝\n");

    printf("\n[F] Avant malloc(100) — appuie sur Entrée\n");
    getchar();
    affiche_maps("avant malloc petit");

    char *petit = malloc(100);
    printf("[F] malloc(100) fait\n");
    affiche_maps("après malloc(100) — heap grandit via brk");
    affiche_ram("après malloc(100)");

    printf("\n[G] On écrit petit[0] = 1\n");
    petit[0] = 1;
    affiche_ram("après petit[0]=1");

    printf("\n[H] free(petit) — heap ne rétrécit PAS forcément\n");
    free(petit);
    affiche_maps("après free(petit) — VMA heap reste là");
    affiche_ram("après free(petit)");

    printf("\nFin. Appuie sur Entrée.\n");
    getchar();
    return 0;
}





// VmSize = tout ce qui est réservé en adresses virtuelles. C'est la somme de toutes les lignes de /proc/maps. Ce n'est pas de la vraie RAM — c'est juste des adresses promises.

// VmRSS = ce qui est réellement en RAM physique en ce moment. C'est ça la vraie consommation mémoire.




// apres le free le vm rss se maj tardivement car seul les majs d allocation calcul la RAM