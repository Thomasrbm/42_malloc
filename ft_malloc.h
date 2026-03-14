
typedef struct s_block
{
    void            *addr;  // adr raw
    struct s_block  *next;
    size_t          size;
    bool            is_free;
}   t_block;

typedef struct s_zone  // global a la liste + block detail pour chaque noed   
{
    void            *start; // adr raw
    size_t          total;
    size_t          used;
    t_zone          *next;  // vers zone suivante de * 100 (128 000)
    t_block         *blocks; // vers debut de la liste de block de 128 ou 1024 etc
}   t_zone;

typedef struct s_malloc
{
    t_zone  *tiny;
    t_zone  *small;
    t_zone  *large;
}   t_malloc;