#include "Literal.hh"
#include "satSolver.hh"
#include <utility>
#include <vector>
#include <iostream>
/*
int main() {
  std::cout << "hello world !" << std::endl;
  return 0;
}
*/


//Renvoie un couple : le premier élément indique si le problème est satisfiable.
//Le cas échéant, le deuxième élément du couple contient une assignation valide.

// en fait y'a déjà des méthodes équivalentes dans satProblem
// on peut récupérer l'alssignation courante,
// et on a une méthode satisfiability, qui renvoie true ssi le probleme est satisfiable,
// et si c'est le cas, laisse l'assignation courante telle qu'elle satisfie les clauses


std::pair<bool, std::vector<bool> > solve(SatProblem problem)
{
  bool isSatisfiable = false;
  std::vector<bool> assignation;






  return std::make_pair(isSatisfiable, assignation);
}
