#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
#include "parser.hh"

#include "Debug.hh"




int main(int argc, char *argv[])
{
    // Pour accélérer les I/O
    //std::ios_base::sync_with_stdio(false);
    if(argc != 2)
    {
        std::cerr << "Nombre d'argument invalide : un unique argument est attendu." << std::endl;
        exit(1);
    }

    std::ifstream input;
    input.open (argv[1], std::ios::in);
    if (!input.is_open())
    {
        std::cerr << "Erreur dans l'ouverture du fichier " << argv[1] << std::endl;
        exit(1);
    }
    // Parse le header dans l'entrée standard : récupère nombre de variables et de clauses
    unsigned int nbrVar, nbrClauses;
    parserHeader(input, nbrVar, nbrClauses);
    // Initialise le problème en lisant l'entrée standard
    SatProblem problem(input, nbrVar, nbrClauses);
    input.close();
    // Résout le problème puis affiche la sortie correspondante
    #if VERBOSE == 0
    if (problem.satisfiability())
        return EXIT_SATISFIABLE;
    else
        return EXIT_UNSATISFIABLE;
    #else
    if(problem.satisfiability())
    {
        std::cout << "s SATISFIABLE\n";
        const std::vector<std::pair<unsigned, bool> > assign = problem.getAssign();
        for(unsigned int k = 0; k < assign.size(); k++)
        {
            std::cout << "v ";
            if (! assign[k].second)
                std::cout << "-";
            std::cout << assign[k].first << '\n';
        }
        return EXIT_SATISFIABLE;
    }
    else
    {
        std::cout << "s UNSATISFIABLE\n";
        return EXIT_UNSATISFIABLE;
    }
    #endif

}



