#include <iostream>
#include <cstdlib>
#include <inttypes.h>

using namespace std;

void skipline(std::istream& read)
{
    std::string tmp;
    std::getline(read, tmp);
}


int main()
{

    int32_t number = 0;
    

    string satState;
    char fooC;
    
    cin >> fooC >> satState;

    if (satState == "UNSATISFIABLE")
    {
        cout << "Aucune solution" << endl;
        return EXIT_FAILURE;
    }

    
    cin >> ws;
    while (cin.peek() == 'v')
    {
        cin >> fooC >> ws; // skip the 'v ' of the beggining of the line
        // ignore la ligne entière des variables fausses
        // ou si la variable ne correspond pas au résultat
        if (cin.peek() == '-' || cin.peek() != 's')
        {
            skipline(cin); 
        }
        else
        {
            cin >> fooC >> fooC; // lit "s_"
            unsigned i;
            cin >> i;
            //cout << "lu bit de résultat " << i << endl;
            number += (1ULL << i);
        }
        cin >> ws;
    }
    cout << number << endl;

    return EXIT_SUCCESS;
}


