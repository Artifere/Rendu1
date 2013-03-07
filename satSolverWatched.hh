#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <utility>

#include <istream>

class Clause;
class BasicClauseWatched;


typedef BasicClauseWatched UsedClause;
//typedef BasicClause UsedClause;

#ifdef INLINED_CLAUSE
typedef UsedClause StockedClause;
#else
typedef Clause StockedClause;
#endif



class SatProblem
{
protected:
    std::vector<varState> _varStates;
    std::vector<unsigned int> _unassignedVarList;
    std::vector<std::pair<std::set<StockedClause*>, std::set<StockedClause*> > > _variables;
    std::vector<std::vector<unsigned int>::iterator> _indexUnassignedList;
    std::set<StockedClause*> _clausesList;

    //True si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,unsigned int> > _stackCallback;

    // ensemble de valeurs à propager (Literaux dont on connaît la valeur).
    // d'intersection vide avec celles déjà assignées (dans _stackCallback), et aucune contradictions entre les déductions
    std::stack<Literal> _deductions; // note : on n'a pas besoin de stoquer la valeur déduite : elle est contenue dans _varStates

    Literal chooseUnasignedVar();
    void deleteUnassignedVar(unsigned int var);
    void addUnassignedVar(unsigned int var);




public:
    friend class BasicClauseWatched;
    
    SatProblem(std::istream& input);
    ~SatProblem();

    bool propagateVariable(const Literal& lit);
    void releaseVariable(const unsigned int varID);

    inline const std::vector<varState>& getAssign() const { return _varStates; }

    void addClause(std::vector<Literal>& lit);
    bool satisfiability();
};





inline Literal SatProblem::chooseUnasignedVar()
{
    unsigned int k = *(_unassignedVarList.end()-1);
    _unassignedVarList.pop_back();
    /*while(k < _varStates.size() && _varStates[k] != FREE)
        k++;*/
    return Literal(k,true);
}

inline void SatProblem::deleteUnassignedVar(unsigned int var)
{
    unsigned int k = *(_unassignedVarList.end()-1);
    _indexUnassignedList[k] = _indexUnassignedList[var];
     *_indexUnassignedList[var] = k;
    _unassignedVarList.pop_back();
    /*for (std::vector<unsigned int>::iterator it = _unassignedVarList.begin(); ; ++it)
    {
        if (*it == var)
        {
            *it = *(_unassignedVarList.end()-1);
            _unassignedVarList.pop_back();
            break;
        }
    }*/
}

inline void SatProblem::addUnassignedVar(unsigned int var)
{
    _indexUnassignedList[var] = _unassignedVarList.end();
    _unassignedVarList.push_back(var);
}

#include "BasicClauseWatched.hh"


#endif//SAT_SOLVER_HH
