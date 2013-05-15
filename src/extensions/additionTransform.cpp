#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>

#define NBR_BITS 32

/*
static inline bool isDigit(char c) {
    return c == '1' || c == '0' || c == '.';
}

// renvoie la suite des opérandes
// elles doivent toutes faire la même longueur
// et être composé seulement de '0' '1' ou '.'
// la dernière opérande est celle de l'autre coté du '='
std::vector<std::string> parseAddition(std::istream& read)

// parse un nombre en binaire avec des 0 1 .
// lance une exception si aucun nombre n'a pu être lu
std::string parseNumber(std::istream& read);

*/

void writeBinary(bool* bits, unsigned number);



int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Nombre incorrect d'arguments : 3 attendus, " << argc << " donnés" << std::endl;
        return EXIT_FAILURE;
    }

    const unsigned nbr1 = atoi(argv[1]);
    const unsigned nbr2 = atoi(argv[2]);
    //std::cout << "c addition de " << nbr1 << " et " << nbr2 << std::endl;
    bool nbrBin1[NBR_BITS];
    bool nbrBin2[NBR_BITS];
    
    writeBinary(nbrBin1, nbr1);
    writeBinary(nbrBin2, nbr2);
    
    // génère le problème correspondant au calcul (sans rien connaître de l'entrée)
    std::cout << "(~ r_0)" << std::endl; 
    for(unsigned i = 0; i < NBR_BITS; i++)
    {
        // s_i <=> (a_i xor b_i xor r_i)  (ie  s_i <=> a_i <=> b_i <=> r_i)
        std::cout << " /\\ (s_"<<i<<" <=> a_"<<i<<" <=> b_"<<i<<" <=> r_"<<i<<")" << std::endl;
        // r_i <=> a_i&b_i | a_i&r_i | b_i&r_i
        std::cout << " /\\ (r_"<<(i+1)<<" <=> ((a_"<<i<<" /\\ b_"<<i<<")  \\/  (a_"<<i<<" /\\ r_"<<i<<")  \\/  (b_"<<i<<" /\\ r_"<<i<<")))" << std::endl;
    }

    // génère les clauses correspondant à l'entrée
    for(unsigned i = 0; i < NBR_BITS; i++)
    {
        std::cout << " /\\ ("<<(nbrBin1[i] ? ' ' : '~')<<"a_"<<i<<")" << std::endl;
        std::cout << " /\\ ("<<(nbrBin2[i] ? ' ' : '~')<<"b_"<<i<<")" << std::endl;
    }
    
    return 0;
}



void writeBinary(bool * bits, unsigned nbr)
{
    for(unsigned b = 0; b < NBR_BITS; b++)
    {
        bits[b] = (nbr & 1) ? true : false;
        nbr >>= 1;
    }
}

/*
std::string parseNumber(std::istream& read)
{
    std::string nbr;
    if (read.eof() || !isDigit(read.peek()))
        throw std::invalid_argument("format d'entrée invalide : '0' '1' ou '.' attendus");
    do {
        char c;
        read >> c;
        nbr += c;
    } while(!read.eof() && isDigit(read.peek()));
    return nbr;
}


std::vector<std::string> parseAddition(std::istream& read)
{
    // lit une entrée au format arg1+arg2=res
    std:string arg1, arg2, res;
    char tmp;

    arg1 = parseNumber(read);

    read >> ws;
    if (read.eof() || read.peek() != '+')
        throw std::invalid_argument("format d'entrée invalide : '+' attendu");
    read >> tmp; // lit le '+'

    arg2 = parseNumber(read);

    read >> ws;
    if (read.eof() || read.peek() != '=')
        throw std::invalid_argument("format d'entrée invalide : '=' attendu");
    read >> tmp; // lit le '='

    res = parseNumber(read);
    
    // vérifie que les trois nombres sont bien sur le même nombre de bit
    if (res.size() != arg1.size() || res.size() != arg2.size())
    {
        throw std::invalid_argument("format d'entrée invalide : les entiers n'ont pas tous le même nombre de bits");
    }
    
    std::vector<std::string> op;
    op.push_back(arg1);
    op.push_back(arg2);
    op.push_back(res);
    return op;
}
*/


