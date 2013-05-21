#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <inttypes.h>

#define NBR_BITS 32


void writeBinary(bool* bits, unsigned number);



int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Nombre incorrect d'arguments : 3 attendus, " << argc << " donnés" << std::endl;
        return EXIT_FAILURE;
    }

    const unsigned long long nbr1 = atoi(argv[1]);
    const unsigned long long nbr2 = atoi(argv[2]);

    bool nbrBin1[NBR_BITS];
    bool nbrBin2[NBR_BITS];
    
    writeBinary(nbrBin1, nbr1); // les nombres sont convertis silencieusement en non signés pour l'addition
    writeBinary(nbrBin2, nbr2); // (le complement à 2 est fait automatiquement)
    
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

