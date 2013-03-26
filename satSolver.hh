#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <stack>
#include <utility>
#include <istream>

#include "Clause.hh"
#include "Literal.hh"
#include "UnassignedBucket.hh"



class SatProblem
{
protected:
    std::vector<Variable*> _variables;
    std::vector<Clause*> _clauses;

    // true si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,Variable*> > _stackBacktrack;

    // heuristique choisie pour choisir une variable non assignée
    //Literal (UnassignedBucket::*const heuristique)(void);

    // ensemble de valeurs à propager (Literaux dont on connaît la valeur).
    // d'intersection vide avec celles déjà assignées (dans _stackCallback), et aucune contradictions entre les déductions
    std::stack<Literal> _deductions; // note : on n'a pas besoin de stoquer la valeur déduite : elle est contenue dans _varStates

    // set of all unassigned Variables (ie complementary set of _stackBacktrack)
    //Pour choisir/mettre à jour la liste des variables non assignées
    UnassignedBucket _unassignedVar;

public:
    SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses);
    ~SatProblem();

    const std::vector<std::pair<unsigned int,varState> > getAssign() const;

    void addClause(CONSTR_ARGS(list));
    bool satisfiability();
};





const std::vector<std::pair<unsigned, varState> > SatProblem::getAssign() const
{
    typedef std::pair<unsigned, varState> return_type;
    std::vector<return_type> res(_variables.size());
    for (unsigned k = 0; k < _variables.size(); k++)
        res[k] = return_type(_variables[k]->varNumber, _variables[k]->_varState);
    return res;
}
        




#endif//SAT_SOLVER_HH
