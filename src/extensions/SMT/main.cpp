#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "UnionFind.hh"
#include "SMT.hh"
#include "Term.hh"
#include "Solution.hh"




int main()
{
    SMT problem = SMT(std::cin);
    bool goOn = true;


    int returnValue;


    while (goOn)
    {
        system("../../../release problem.cnf > result.txt");
        std::ifstream resInput;
        resInput.open("result.txt");

        char fooC;
        std::string resSAT, fooS;

        while (resInput.peek() == 'c')
        {
            std::getline(resInput, fooS);
        }



        resInput >> fooC >> resSAT;
        
        
        if (resSAT == "UNSATISFIABLE")
        {
            std::cout << "Le problème est insatisfiable." << std::endl;
            returnValue = 20;
            goOn = false;
        }
        else
        {
            problem.readSolverAssignations(resInput);
            Solution sol(problem);
            if (sol.check())
            {
                std::cout << "Le problème est satisfiable.\n";
                problem.printResult();
                goOn = false;
                returnValue = 10;
            }
			/* Il faut ajouter des clauses pour préciser les conditions de congruence */
            else
            {
                std::ifstream inProblem;
                system("cp problem.cnf problem.back"); 
                inProblem.open("problem.back");
                std::ofstream outProblem;
                outProblem.open("problem.cnf", std::ofstream::trunc);

				/* On réécrit le problème, en augmentant le nombre de clauses de 1 : on ajoute une clause expliquand le conflit */
                unsigned nbrVar, nbrClauses;
                std::string fooS;
                inProblem >> fooS >> fooS >> nbrVar >> nbrClauses;
                outProblem << "p cnf " << nbrVar << " " << nbrClauses+1 << std::endl;
               
                char fooC;
                inProblem >> std::noskipws >> fooC;
                for (unsigned i = 0; i < nbrClauses; i++)
                {
                    std::getline(inProblem, fooS);
                    outProblem << fooS << '\n';
                }
                inProblem.close();
				system("rm problem.back");
				/* Ajout de la clause expliquand le conflit */
                sol.printNewClause(outProblem);
                outProblem.close();
            }
        }

        resInput.close();
    }


    return returnValue;
}


