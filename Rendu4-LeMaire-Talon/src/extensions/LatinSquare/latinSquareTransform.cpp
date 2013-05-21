#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;


int main(int argc, char *argv[])
{
    unsigned lin, col, nb, nb2, col2, lin2;
    bool noExtraCond = false;

    if (argc == 3 && string(argv[2]) == "--no-extra-cond")
    {
        argc --;
        noExtraCond = true;
    }

    if (argc != 2)
    {
        cerr << "Nombre d'arguments invalide" << endl;
        return -1;
    }


    const unsigned k = atoi(argv[1]);

    /* Premières conditions : au moins un nombre par case */
    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k; col++)
        {
            if (lin != 0 || col != 0)
                cout << " /\\ ";
            cout << '(';
            for (nb = 0; nb < k-1; nb++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << lin << "_" << col << "_" << k-1 << ")" << endl;
        }
    }

    cout << endl << endl << endl;

    /* Maintenant on s'occupe de vérifier que chaque ligne a tous les nombres */
    for (lin = 0; lin < k; lin++)
    {
        for (nb = 0; nb < k; nb++)
        {
            cout << "/\\ (";
            for (col = 0; col < k-1; col++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << lin << "_" << k-1 << "_" << nb << ")" << endl;
        }
    }

    cout << endl;

    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (col = 0; col < k; col++)
    {
        for (nb = 0; nb < k; nb++)
        {
            cout << "/\\ (";
            for (lin = 0; lin < k-1; lin++)
            {
                cout << "ls_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "ls_" << k-1 << "_" << col << "_" << nb << ")" << endl;
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
                    cout << "/\\ (~ls_" << lin << "_" << col << "_" << nb << " \\/ ";
                    cout << "~ls_" << lin << "_" << col << "_" << nb2 << ")";
                    cout << endl;
                }
            }
        }
    }



    /* Conditions superflues mais accélérant grandement la résolution */

    if (noExtraCond)
        return 0;

    /* Pas deux fois le même nombre sur une ligne */

    for (lin = 0; lin < k; lin++)
    {
        for (nb = 0; nb < k; nb++)
        {
            for (col = 0; col < k-1; col++)
            {
                for (col2 = col+1; col2 < k; col2++)
                {
                    cout << "/\\ (~ls_" << lin << "_" << col << "_" << nb << " \\/ ";
                    cout << "~ls_" << lin << "_" << col2 << "_" << nb << ")";
                    cout << endl;
                }
            }
        }
    }

    /* Pas deux fois le même nombre sur une colonne */

    for (col = 0; col < k; col++)
    {
        for (nb = 0; nb < k; nb++)
        {
            for (lin = 0; lin < k-1; lin++)
            {
                for (lin2 = lin+1; lin2 < k; lin2++)
                {
                    cout << "/\\ (~ls_" << lin << "_" << col << "_" << nb << " \\/ ";
                    cout << "~ls_" << lin2 << "_" << col << "_" << nb << ")";
                    cout << endl;
                }
            }
        }
    }


    return 0;
}
