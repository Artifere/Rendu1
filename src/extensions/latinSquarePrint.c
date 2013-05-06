#include "stdio.h"
#include "stdlib.h"



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Nombre incorrect d'arguments\n");
        exit(0);
    }

    const unsigned k = atoi(argv[1]);


    unsigned **square = NULL;
    square = malloc(k*sizeof(*square));
    unsigned lin, col, nb;
    for (lin = 0; lin < k; lin++)
        square[lin] = malloc(k*sizeof(*square[lin]));


    char c;
    do
    {
        c = getchar();
        if (c == '-')
            getchar();
        
        scanf("x_%d_%d_%d\n", &lin, &col, &nb);
        
        if (c != '-')
            square[lin][col] = nb;
    } while (c != EOF);




    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k-1; col++)
        {
            printf("%d\t", square[lin][col]);
        }
        printf("%d\n", square[lin][k-1]);
    }



    for (lin = 0; lin < k; lin++)
        free(square[lin]);
    free(square);


    return EXIT_SUCCESS;
}


