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
    std::vector<unsigned int> _unassignedVarList;
    std::vector<std::pair<std::set<Clause*>, std::set<Clause*> > > _variables;
    std::vector<std::vector<unsigned int>::iterator> _indexUnassignedList;
    //True si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,unsigned int> > _stackCallback;
    
    // ensemble de valeurs à propager (Literaux dont on connaît la valeur).
    // d'intersection vide avec celles déjà assignées (dans _stackCallback), et aucune contradictions entre les déductions
    std::stack<Literal> _deductions; // note : on n'a pas besoin de stoquer la valeur déduite : elle est contenue dans _varStates

    inline Literal chooseUnasignedVar()
    {
        unsigned int k = *(_unassignedVarList.end()-1);
        _unassignedVarList.pop_back();
        /*while(k < _varStates.size() && _varStates[k] != FREE)
            k++;*/

        return Literal(k,true);
    }

    inline void deleteUnassignedVar(unsigned int var)
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

    inline void addUnassignedVar(unsigned int var)
    {
        _indexUnassignedList[var] = _unassignedVarList.end();
        _unassignedVarList.push_back(var);
    }

public:
    SatProblem(std::istream& input);
    ~SatProblem();
    bool deduceFromSizeOne();

    inline const std::vector<varState>& getAssign() const
    {
        return _varStates;
    }
    
    void addClause(std::vector<Literal>& lit);
    inline void propagationTrue(Literal lit, std::set<Clause*>& clauseSet)
    {
        for (std::set<Clause*>::iterator it = clauseSet.begin(); it != clauseSet.end(); ++it)
            // on passe la clause à true : pas besoin de tester une déduction où une contradiction
        (*it)->setLitTrue(lit);
    }
    bool propagationFalse(Literal lit, std::set<Clause*>& clauseSet); 
    bool satisfiability();
};







#endif//SAT_SOLVER_HH
