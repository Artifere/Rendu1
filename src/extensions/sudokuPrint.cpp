#include <iostream>


using namespace std;


int main(void)
{
    unsigned grid[9][9];
    unsigned lin, col, nb;


	string satState;
    char c, fooC;
    
    cin >> fooC >> satState;

    if (satState == "UNSATISFIABLE")
    {
        cout << "Aucune solution" << endl;
        return 1;
    }


    cin >> fooC;
    do
    {
        cin >> c;
        if (c == '-')
            cin >> fooC;
        cin >> fooC >> lin >> fooC >> col >> fooC >> nb;
        if (c != '-')
            grid[lin][col] = nb;
        cin >> fooC;
    } while (!cin.eof());


    /* On affiche la solution */
    for (lin = 0; lin < 9; lin++)
    {
        for (col = 0; col < 8; col++)
        {
            cout << grid[lin][col] << "\t";
        }
        cout << grid[lin][8] << endl;
    }



    return 0;
}


