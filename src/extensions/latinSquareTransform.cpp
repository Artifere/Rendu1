#include <iostream>
#include <cstdlib>

using namespace std;


int main(int argc, char *argv[])
{
    unsigned lin, col, nb, nb2;
    if (argc != 2)
    {
        cerr << "Nombre d'arguments invalide" << endl;
        return -1;
    }


    const unsigned k = atoi(argv[1]);
    /* A priori, k <= 2^10 = 1024, ça fait environ 5*10^11 clauses => plus d'une heure
       pour écrire le fichier de sortie => non */

    /* Premières conditions : au moins un nombre par case */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            cout << '(';
            for (nb = 0; nb < k-1; nb++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << lin << "_" << col << "_" << k-1 << ") /\\" << endl;
        }
    }

    cout << endl << endl << endl;


    /* Maintenant on s'occupe de vérifier que chaque ligne a tous les nombres */
    for (lin = 0; lin < k; lin++)
    {
        for (nb = 0; nb < k; nb++)
        {
            cout << '(';
            for (col = 0; col < k-1; col++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << lin << "_" << k-1 << "_" << nb << ") /\\" << endl;
        }
    }

    cout << endl;

    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (col = 0; col < k; col++)
    {
        for (nb = 0; nb < k; nb++)
        {
            cout << '(';
            for (lin = 0; lin < k-1; lin++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << k-1 << "_" << col << "_" << nb << ") /\\" << endl;
        }
    }

    cout << endl << endl;

    /* Maintenant on vérifie qu'une case a un seul nombre */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            for (nb = 0; nb < k-1; nb++)
            {
                for (nb2 = nb+1; nb2 < k; nb2++)
                {
                    cout << "(~ls_" << lin << "_" << col << "_" << nb << " \\/ ";
                    cout << "~ls_" << lin << "_" << col << "_" << nb2 << ")";
                    if (!(lin == k-1 && col == k-1 && nb == k-2))
                        cout << " /\\";
                    cout << endl;
                }
            }
        }
    }

    return 0;
}
