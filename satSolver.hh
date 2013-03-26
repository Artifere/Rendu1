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

    // Vaut true si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,Variable*> > _stackBacktrack;

    /* Ensemble de valeurs à propager (literaux dont on connaît la valeur).
       Il est d'intersection vide avec celles déjà assignées (dans _stackCallback), et il n'y aaucune contradiction entre les déductions.
       Remarque : on n'a pas besoin de stoquer la valeur déduite : elle est contenue dans _varStates */
    std::stack<Literal> _deductions; 

    // Ensemble des variables non assignées ni en cours de déductions, utilisées pour les « paris »
    UnassignedBucket _unassignedVar;

public:
    SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses);
    ~SatProblem();

    inline const std::vector<std::pair<unsigned int,varState> > getAssign(void) const;

    void addClause(CONSTR_ARGS(list));
    bool satisfiability();
};




// Appelée à la fin du programme dans le cas où le problème est satisfiable
inline const std::vector<std::pair<unsigned, varState> > SatProblem::getAssign(void) const
{
    typedef std::pair<unsigned, varState> return_type;
    std::vector<return_type> res(_variables.size());

    for (unsigned k = 0; k < _variables.size(); k++)
        res[k] = return_type(_variables[k]->varNumber, _variables[k]->_varState);
    return res;
}
        




#endif // SAT_SOLVER_HH defined
