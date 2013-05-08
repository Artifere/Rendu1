#include "stdio.h"
#include "stdlib.h"



int main(void)
{
    unsigned **grid = NULL;
    grid = malloc(9*sizeof(*grid));
    unsigned lin, col, nb;
    for (lin = 0; lin < 9; lin++)
        grid[lin] = malloc(9*sizeof(*grid[lin]));



	char satState[14];
	char c;
	getchar();
	getchar();
    scanf("%s\n", satState);
    if (satState == "UNSATISFIABLE")
    {
        printf("Aucune solution\n");
        return 0;
    }


    
    getchar();
    do
    {
        getchar();
        c = getchar();
        getchar();
        scanf("_%d_%d_%d\n", &lin, &col, &nb);
        
        if (c != '-')
            grid[lin][col] = nb;
        c = getchar();
    } while (c != EOF);




    for (lin = 0; lin < 9; lin++)
    {
        for (col = 0; col < 8; col++)
        {
            printf("%d\t", grid[lin][col]);
        }
        printf("%d\n", grid[lin][8]);
    }



    for (lin = 0; lin < 9; lin++)
        free(grid[lin]);
    free(grid);


    return EXIT_SUCCESS;
}


