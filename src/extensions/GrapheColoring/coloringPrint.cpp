#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


using namespace std;

//Ignore les commentaires
void skipComments(std::istream& input)
{
    string read;
    while((input >> std::ws) && (input.peek()=='c'))
        getline(input, read);
}


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Nombre incorrect d'arguments" << endl;
        return -1;
    }

    const unsigned k = atoi(argv[1]);
    if (k > 30)
    {
        cerr << "Seulement 30 couleurs sont disponibles pour colorier le graphe." << endl;
        return -1;
    }
    string colorNames[30] = {"blue", "red", "green", "yellow", "purple", "brown", "orange",
                           "grey", "cyan", "chartreuse", "pink", "violet", "marron",
                           "navy", "peachpuff", "darkolivegreen", "teal", "oranged2",
                           "plum", "seagreen", "gold2", "darkgoldenrod3", "burlywood2",
                           "gray99", "palegreen2", "olivedrab2", "royalblue",
                           "snow", "violetred", "turquoise1"};

    cout << "strict graph G {" << endl;
    ifstream input;
    input.open(argv[3]);
    
    int node, col;

	string satState;
    char c, fooC;
    
    input >> fooC >> satState;

    if (satState == "UNSATISFIABLE")
    {
        cout << "Aucune solution" << endl;
        return 1;
    }

    input >> fooC;
    do
    {
        input >> c;
        if (c == '-')
            input >> fooC;
        input >> fooC >> node >> fooC >> col;
        if (c != '-')
            cout << node << "[style=filled, color =" << colorNames[col] << "];" << endl;
        input >> fooC;
    } while (!input.eof());

    input.close();


    input.open(argv[2]);
    skipComments(input);


    string fooS;
    input >> c >> fooS;
    if (c != 'p' || fooS != "edge")
    {
        cerr << "Attention, fichier de description de graphe invalide" << endl;
        return -1;
    }

    int nbNodes, nbEdges;
    int n1, n2;
    input  >> nbNodes >> nbEdges;

    for (int i = 0; i < nbEdges; i++)
    {
        input >> c >> n1 >> n2;
        cout << n1 << "--" << n2 << ";" << endl;
    }

    cout << "}" << endl;
    input.close();
    
    return 0;
}


