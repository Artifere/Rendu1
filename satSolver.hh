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
    std::set<unsigned int> _unassignedVarList;
    std::vector<std::pair<std::set<Clause*>, std::set<Clause*> > > _variables;
    std::vector<varState> _deducedState;
    //std::stack<std::pair<bool,Literal> > _stackCallback;

    //True si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,unsigned int> > _stackCallback; 
    Literal chooseUnasignedVar();
    

public:
    SatProblem(std::istream& input);
    ~SatProblem();
    bool deduceFromSizeOne();

    inline const std::vector<varState>& getAssign() const
    {
        return _varStates;
    }
    bool satisfiability();
};







#endif//SAT_SOLVER_HH
