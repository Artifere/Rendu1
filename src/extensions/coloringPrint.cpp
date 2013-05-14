#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void skipComments(void)
{
    int c = getchar();
    while (c == 'c')
    {
        while (c != '\n')
            c = getchar();
        c = getchar();
    }

    ungetc(c, stdin);
}



int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Nombre incorrect d'arguments\n");
        exit(0);
    }

    const unsigned k = atoi(argv[1]);
    char colorNames[20][50] = {"blue", "red", "green", "yellow", "purple", "brown", "orange",
                           "grey", "cyan", "chartreuse", "pink", "violet", "marron",
                           "navy", "peachpuff", "darkolivegreen", "teal", "oranged2",
                           "plum", "seagreen"};

    //FILE *graphDescription, *nodesColors; 
    printf("strict graph G {\n");    
    freopen(argv[3], "r", stdin);
    
    char c;
    int node, col;
    
    c = getchar();
    if (c != 's')
    {
        fprintf(stderr, "Erreur, fichier d'assignation incorrect\n");
        exit(1);
    }

    char satState[14];
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
        getchar();
        scanf("%d_%d\n", &node, &col);
        
        if (c != '-')
            printf("%d[color=%s];\n", node, colorNames[col]);
        c = getchar();
    } while (c != EOF);
 


    freopen(argv[2], "r", stdin);
    skipComments();
    c = getchar();
    if (c != 'p')
    {
        fprintf(stderr, "ATTENTION 3 : %c\n", c);
        exit(3);
    }
    c = getchar();

    char foo[20];
    scanf("%s", foo);
    if (strcmp(foo, "edge") != 0)
    {
        fprintf(stderr, "ATTENTION 4 : %s\n", foo);
        exit(4);
    }

    int nbNodes, nbEdges;
    int n1, n2;
    scanf("%d %d", &nbNodes, &nbEdges);


    for (int i = 0; i < nbEdges; i++)
    {
        scanf("%c %d %d", &c, &n1, &n2);
        printf("%d--%d;\n", n1, n2);
    }

    printf("}\n");
    
    return EXIT_SUCCESS;
}


