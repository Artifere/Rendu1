#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>


//TODO: un set avec les aretes : a < b ssi min(a1, a2) < min(b1, b2), max....


// Ignore les commentaires
void skipComment(std::istream& input)
{
    std::string read;
    while((input >> std::ws) && (input.peek()=='c'))
        std::getline(input, read);
}
    
int main(int argc, char *argv[])
{
	if (argc != 2)
    {
        std::cout << "Nombre d'arguments invalide\n";
        exit(1);
    }
	char c;
    std::string foo;
    unsigned nbNodes, nbEdges;

    const unsigned k = atoi(argv[1]);

    skipComment(std::cin);
    if (!(std::cin >> c >> foo >> nbNodes >> nbEdges) || c != 'p')
    {
        std::cout << "c Arg, entrée invalide\n";
        exit(1);
    }

    std::vector< std::pair<unsigned, unsigned> >  neighbors;
	neighbors.reserve(nbEdges);

    unsigned n1, n2;
    skipComment(std::cin);
    
    /* Les conditions sur la propreté du coloriage */
    while (std::cin.peek() != EOF)
    {
        std::cin >> c >> n1 >> n2;
        if (c != 'e')
        {
            std::cout << "Entrée invalide\n";
            exit(2);
        }
		
		neighbors.push_back(std::make_pair(n1,n2));

        for (unsigned color = 0; color < k; color++)
            std::cout << "~(c_" << n1  << "_" << color << "/\\c_" << n2 << "_" << color << ") /\\\n";
        skipComment(std::cin);
    }
	std::cout << "\n\n";
	
	
	/* On impose qu'un noeud n'ait pas deux couleurs */
	for (unsigned node = 1; node <= nbNodes; node++)
		for (unsigned c1 = 0; c1 < k-1; c1++)
			for (unsigned c2 = c1+1; c2 < k; c2++)
				std::cout << "~(c_" << node << "_" << c1 << "/\\c_" << node << "_" << c2 << ") /\\\n";
	
	std::cout << "\n\n";
	
	
	/* On impose qu'un noeud ait une couleur */
	for (unsigned node = 1; node <= nbNodes; node++)
    {
		std::cout << '(';
		for (unsigned color = 0; color < k-1; color++)
		{
			std::cout << "c_" << node << "_" << color << "\\/";
		}
		std::cout << "c_" << node << "_" << k-1 << ')';
		if (node < nbNodes)
			std::cout << "/\\";
		std::cout << "\n";
    }
	
	
	return EXIT_SUCCESS;
}
	
	
	
	
