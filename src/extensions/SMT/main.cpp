#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "UnionFind.hh"
#include "SMT.hh"
#include "Term.hh"


int main()
{
    SMT foo = SMT(std::cin);
    
    foo.miscTests();


    bool encore = true;





    while (encore)
    {
        encore = false; //TODO
        system("../../../release problem.cnf > result.txt");
        std::ifstream resInput;
        resInput.open("result.txt");

        char fooC;
        std::string resSAT;
        resInput >> fooC >> resSAT;
        if (resSAT == "UNSATISFIABLE")
        {
            std::cout << "Le problème est insatisfiable.\n";
        }
        else
        {
            foo.readSolverAssignations(resInput);
            
            
            if (true/*check(blabla*/)
            {
                std::cout << "Le problème est satisfiable.\n";
                foo.printResult();
                encore = false;
            }
        }

        resInput.close();
    }


    return 0;
}
