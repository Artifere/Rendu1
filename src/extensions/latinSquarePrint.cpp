#include <iostream>
#include <string>
#include <cstdlib>


using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Nombre incorrect d'arguments" << endl;
        return -1;
    }

    const unsigned k = atoi(argv[1]);


    unsigned **square = new unsigned*[k];
    for (unsigned i = 0; i < k; i++)
        square[i] = new unsigned[k];
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
        cin >> c >> fooC;
        if (c == '-')
            cin >> fooC;
        cin >> fooC >> lin >> fooC >> col >> fooC >> nb;
        if (c != '-')
            square[lin][col] = nb;
        cin >> fooC;
    } while (!cin.eof());




    for (lin = 0; lin < k; lin++)
    {
        for (col = 0; col < k-1; col++)
        {
            cout << square[lin][col] << "\t";
        }
        cout << square[lin][k-1] << endl;
    }



    for (unsigned i = 0; i < k; i++)
        delete[] square[i];
    delete[] square;

    return 0;
}


