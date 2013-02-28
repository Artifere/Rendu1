#ifndef SOLVER_HH
#define SOLVER_HH


#include "Literal.hh"
#include "satSolver.hh"
#include <utility>
#include <vector>


//Renvoie un couple : le premier élément indique si le problème est satisfiable.
//Le cas échéant, le deuxième élément du couple contient une assignation valide.
std::pair<bool, std::vector<bool> > solve(SatProblem& problem);


#endif // SOLVER_HH included
