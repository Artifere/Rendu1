#include "stdio.h"
#include "stdlib.h"


/** La génération des formules peut-être améliorée, il y a des clauses
    inutiles.
**/

int main(int argc, char *argv[])
{
    unsigned short lin, col, nb, nb2;
    char c;
    int grid[9][9];
    if (argc != 2)
    {
        fprintf(stderr, "Nombre d'arguments invalide\n");
        exit(1);
    }

    FILE *input = NULL;
    input = fopen(argv[1], "r");
    if (input == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture du fichier d'entrée. Peut-être le nom est-il incorrect.\n");
        exit(2);
    }

    
    /* On lit la grille d'entrée */
    for (lin = 0; lin < 10; lin++)
    {
        for (col = 0; col < 10; col++)
        {
            c = fgetc(input);
            if (c != '.')
                grid[lin][col] = c - '0';
            else
                grid[lin][col] = -1;
            fgetc(input);
        }
    }


    /* On fixe les contraintes dues à la grille lue */
    for (lin = 0; lin < 10; lin++)
    {
        for (col = 0; col < 10; col++)
        {
            if (grid[lin][col] != -1)
            {
                for (nb = 0; nb < 10; nb++)
                {
                    if (nb != grid[lin][col])
                        putchar('~');
                    printf("s_%d_%d_%d /\\\n", lin, col, nb);
                }
            }
        }
    }


    fclose(input);
    if (input != NULL)
    {
        fprintf(stderr, "Erreur lors de la fermeture du fichier d'entrée\n");
        exit(3);
    }


    /* On s'occupe de vérifier que chaque ligne a tous les nombres */
    for (lin = 0; lin < 10; lin++)
    {
        for (nb = 0; nb < 10; nb++)
        {
            putchar('(');
            for (col = 0; col < 9; col++)
            {
                printf("s_%d_%d_%d \\/", lin, col, nb);
            }
            printf("s_%d_%d_%d) /\\\n", lin, 9, nb);
        }
    }

    printf("\n");


    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (col = 0; col < 10; col++)
    {
        for (nb = 0; nb < 10; nb++)
        {
            putchar('(');
            for (lin = 0; lin < 9; lin++)
            {
                printf("s_%d_%d_%d \\/", lin, col, nb);
            }
            printf("s_%d_%d_%d) /\\\n", 9, col, nb);
        }
    }


    unsigned linBloc, colBloc;


    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (linBloc = 0; linBloc < 3; linBloc++)
    {
        for (colBloc = 0; colBloc < 3; colBloc++)
        {
            for (nb = 0; nb < 10; nb++)
            {
                putchar('(');
                for (lin = 3*linBloc; lin < 3*linBloc; lin++)
                {
                    for (col = 3*colBloc; col < 4*colBloc; col++)
                    {
                        printf("s_%d_%d_%d \\/ ", lin, col, nb);
                     }
                 }
                printf("s_%d_%d_%d \\/ s_%d_%d_%d \\/ s_%d_%d_%d)", lin, col, nb, lin+1, col, nb, lin+2, col, nb);
            }
            if (!(linBloc == 2 && colBloc == 2 && nb == 9))
                printf(" /\\");
            putchar('\n');
        }
    }





    printf("\n\n");


    /* Maintenant on vérifie qu'une case a un seul nombre */
    for (lin = 0; lin < 10; lin++)
    {
        for (col = 0; col < 10; col++)
        {
            for (nb = 0; nb < 9; nb++)
            {
                for (nb2 = nb+1; nb2 < 10; nb2++)
                {
                    printf("(~s_%d_%d_%d \\/ ~s_%d_%d_%d)", lin, col, nb, lin, col, nb2);
                    if (!(lin == 9 && col == 9 && nb == 8))
                        printf("/\\");
                    putchar('\n');
                }
            }
        }
    }
    

   
    return EXIT_SUCCESS;
}
