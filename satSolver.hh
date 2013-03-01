#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <utility>
#include "Clause.hh"
#include <istream>

typedef enum varState
{
    TRUE, FALSE, FREE
} varState;


class SatProblem
{
protected:
    std::vector<varState> _varStates;
    std::vector<std::pair<std::set<Clause*>, std::set<Clause*> > > _variables;
    //std::stack<std::pair<bool,Literal> > _stackCallback;
    std::stack<std::pair<bool,unsigned int> > _stackCallback; //Je pense que c'est mieux. le bool c'est pour dire si on peut changer la valeur de la variable ou si c'était un choix nécessaire
    //l'autre paramètre c'est le numéro de la variable

    Literal chooseUnasignedVar() const;

public:
    SatProblem(std::istream& input);
    ~SatProblem();

    inline const std::vector<varState>& getAssign() const
    {
        return _varStates;
    }
    bool satisfiability();
};







#endif//SAT_SOLVER_HH
