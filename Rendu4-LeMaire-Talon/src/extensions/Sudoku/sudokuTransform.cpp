#include <iostream>
#include <fstream>

using namespace std;
/** La génération des formules peut-être améliorée, il y a des clauses
    inutiles.
**/

int main(int argc, char *argv[])
{
    unsigned short lin, col, nb, nb2;
    char c;
    short grid[9][9];
    if (argc != 2)
    {
        cerr << "Nombre d'arguments invalide" << endl;
        return -1;
    }

    ifstream input;
    input.open(argv[1]);
    
    
    
    /* On lit la grille d'entrée */
    for (lin = 0; lin < 9; lin++)
    {
        for (col = 0; col < 9; col++)
        {
            input >> c;
            if (c != '.')
                grid[lin][col] = c - '0';
            else
                grid[lin][col] = -1;
            input >> c;
        }
        
    }

    input.close();
    /* On fixe les contraintes dues à la grille lue */
    for (lin = 0; lin < 9; lin++)
    {
        for (col = 0; col < 9; col++)
        {
            if (grid[lin][col] != -1)
            {
                cout << "s_" << lin << "_" << col << "_" << grid[lin][col] << " /\\" << endl;
                for (nb = 1; nb <= 9; nb++)
                    if (grid[lin][col] != nb)
                        cout << "~s_" << lin << "_" << col << "_" << nb << " /\\" << endl;
            }
        }
    }

    cout << endl << endl;
    /* On s'occupe de vérifier que chaque ligne a tous les nombres */
    for (lin = 0; lin < 9; lin++)
    {
        for (nb = 1; nb <= 9; nb++)
        {
            cout << '(';
            for (col = 0; col < 8; col++)
            {
                cout << "s_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "s_" << lin << "_8_" << nb << ") /\\" << endl;
        }
    }

    cout << endl;
    
    
    /* Maintenant on s'occupe de vérifier que chaque colonne a tous les nombres */
    for (col = 0; col < 9; col++)
    {
        for (nb = 1; nb <= 9; nb++)
        {
            cout << '(';
            for (lin = 0; lin < 8; lin++)
            {
                cout << "s_" << lin << "_" << col << "_" << nb << " \\/";
            }
            cout << "s_8_" << col << "_" << nb << ") /\\" << endl;
        }
    }

    cout << endl;

    unsigned linBloc, colBloc;


    /* Maintenant on s'occupe de vérifier que chaque bloc a tous les nombres */
    for (linBloc = 0; linBloc < 3; linBloc++)
    {
        for (colBloc = 0; colBloc < 3; colBloc++)
        {
            for (nb = 1; nb <= 9; nb++)
            {
                cout << '(';
                for (lin = 3*linBloc; lin < 3*linBloc+2; lin++)
                {
                    for (col = 3*colBloc; col < 3*colBloc+3; col++)
                    {
                        cout << "s_" << lin << "_" << col << "_" << nb << " \\/ ";
                    }
                 }
                col = 3*colBloc;
                cout << "s_" << lin << "_" << col << "_" << nb << " \\/";
                cout << "s_" << lin << "_" << col+1 << "_" << nb << " \\/";
                cout << "s_" << lin << "_" << col+2 << "_" << nb << ") /\\" << endl;
                
            }
            cout << endl;
        }
    }





    cout << endl << endl;

    /* Maintenant on vérifie qu'une case a un seul nombre */
    for (lin = 0; lin < 9; lin++)
    {
        for (col = 0; col < 9; col++)
        {
            for (nb = 1; nb < 9; nb++)
            {
                for (nb2 = nb+1; nb2 <= 9; nb2++)
                {
                    cout << "(~s_" << lin << "_" << col << "_" << nb << " \\/ ";
                    cout << "~s_" << lin << "_" << col << "_" << nb2 << ")";
                    
                    if (!(lin == 8 && col == 8 && nb == 8 && nb2 == 9))
                        cout << " /\\";
                    cout << endl;
                }
            }
        }
    }
    

   
    return 0;
}
