#include "stdio.h"
#include "stdlib.h"


int main(int argc, char *argv[])
{
    unsigned lin, col, nb, nb2;
    if (argc != 2)
    {
        fprintf(stderr, "Nombre d'arguments invalide\n");
        exit(1);
    }


    const unsigned k = atoi(argv[1]);
    const unsigned kCarre = k*k;
    /* A priori, k <= 2^10 = 1024, ça fait plus de 5*10^17 clauses => plus de 10 ans
       pour écrire le fichier de sortie => non */

    /* Premières conditions : au moins un nombre par case */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            putchar('(');
            for (nb = 0; nb < kCarre-1; nb++)
            {
                printf("ms_%d_%d_%d \\/", lin, col, nb);
            }
            printf("ms_%d_%d_%d) /\\\n", lin, col, kCarre);
        }
    }

    printf("\n\n\n");




    /* Maintenant on vérifie qu'une case a un seul nombre */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            for (nb = 0; nb < kCarre-1; nb++)
            {
                for (nb2 = nb+1; nb2 < kCarre; nb2++)
                {
                    printf("(~ms_%d_%d_%d \\/ ~ms_%d_%d_%d)", lin, col, nb, lin, col, nb2);
                    if (!(lin == k-1 && col == kCarre-1 && nb == kCarre-2))
                        printf("/\\");
                    putchar('\n');
                }
            }
        }
    }


    printf("TODO: conditions sur les sommes...\n");
    return EXIT_SUCCESS;
}
