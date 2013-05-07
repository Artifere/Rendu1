#include "stdio.h"
#include "stdlib.h"



int main(int argc, char *argv[])
{
    unsigned **grid = NULL;
    grid = malloc(10*sizeof(*grid));
    unsigned lin, col, nb;
    for (lin = 0; lin < 10; lin++)
        grid[lin] = malloc(10*sizeof(*grid[lin]));


    char c;
    do
    {
        c = getchar();
        if (c == '-')
            getchar();
        scanf("_%d_%d_%d\n", &lin, &col, &nb);
        
        if (c != '-')
            grid[lin][col] = nb;
    } while (c != EOF);




    for (lin = 0; lin < 10; lin++)
    {
        for (col = 0; col < 9; col++)
        {
            printf("%d\t", grid[lin][col]);
        }
        printf("%d\n", grid[lin][9]);
    }



    for (lin = 0; lin < 10; lin++)
        free(grid[lin]);
    free(grid);


    return EXIT_SUCCESS;
}


