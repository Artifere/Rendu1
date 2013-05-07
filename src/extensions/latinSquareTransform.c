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
    /* A priori, k <= 2^10 = 1024, ça fait environ 5*10^11 clauses => plus d'une heure
       pour écrire le fichier de sortie => non */

    /* Premières conditions : au moins un nombre par case */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            putchar('(');
            for (nb = 0; nb < k-1; nb++)
            {
                printf("ls_%d_%d_%d \\/", lin, col, nb);
            }
            printf("ls_%d_%d_%d) /\\\n", lin, col, k-1);
        }
    }

    printf("\n\n\n");


    /* Maintenant on s'occupe de vérifier que chaque ligne a tous les nombres */
    for (lin = 0; lin < k; lin++)
    {
        for (nb = 0; nb < k; nb++)
        {
            putchar('(');
            for (col = 0; col < k-1; col++)
            {
                printf("ls_%d_%d_%d \\/", lin, col, nb);
            }
            printf("ls_%d_%d_%d) /\\\n", lin, k-1, nb);
        }
    }

    printf("\n");


    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (col = 0; col < k; col++)
    {
        for (nb = 0; nb < k; nb++)
        {
            putchar('(');
            for (lin = 0; lin < k-1; lin++)
            {
                printf("ls_%d_%d_%d \\/", lin, col, nb);
            }
            printf("ls_%d_%d_%d) /\\\n", k-1, col, nb);
        }
    }

    printf("\n\n");


    /* Maintenant on vérifie qu'une case a un seul nombre */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            for (nb = 0; nb < k-1; nb++)
            {
                for (nb2 = nb+1; nb2 < k; nb2++)
                {
                    printf("(~ls_%d_%d_%d \\/ ~ls_%d_%d_%d)", lin, col, nb, lin, col, nb2);
                    if (!(lin == k-1 && col == k-1 && nb == k-2))
                        printf("/\\");
                    putchar('\n');
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
