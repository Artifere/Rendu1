#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <utility>
#include "Clause.hh"
#include "Literal.hh"
#include <istream>




class SatProblem
{
protected:
    std::vector<Variable*> _variables;
    std::vector<StockedClause*> _clauses;
	 
	 //Pour la version rapide
	 std::vector<unsigned int> _unassignedVarList;
	 std::vector<std::vector<unsigned int>::iterator> _indexUnassignedList;
    
    // true si on peut changer la valeur, false si c'était un choix contraint
    std::stack<std::pair<bool,Variable*> > _stackBacktrack;
    // set of all unassigned Variables (ie complementary set of _stackBacktrack)
    std::set<Variable*> _unassignedVarPool;

    // ensemble de valeurs à propager (Literaux dont on connaît la valeur).
    // d'intersection vide avec celles déjà assignées (dans _stackCallback), et aucune contradictions entre les déductions
    std::stack<Literal> _deductions; // note : on n'a pas besoin de stoquer la valeur déduite : elle est contenue dans _varStates

    Literal chooseUnasignedVar();
    void deleteUnassignedVar(Variable* var);
    void addUnassignedVar(Variable* var);

public:
    SatProblem(std::istream& input);
    ~SatProblem();

    const std::vector<std::pair<unsigned int,varState> > getAssign() const;

    void addClause(std::vector<Literal>& lit, unsigned);
    bool satisfiability();
};





const std::vector<std::pair<unsigned int,varState> > SatProblem::getAssign() const
{
    std::vector<std::pair<unsigned int,varState> > res(_variables.size());
    for (unsigned k = 0; k < _variables.size(); k++)
        res[k] = std::pair<unsigned int, varState>(_variables[k]->varNumber, _variables[k]->_varState);
    return res;
}
        



//Version rapide

inline Literal SatProblem::chooseUnasignedVar()
{
    unsigned int k = *(_unassignedVarList.end()-1);
    _unassignedVarList.pop_back();
    return Literal(_variables[k],true);
}

inline void SatProblem::deleteUnassignedVar(Variable* var)
{
    unsigned int k = *(_unassignedVarList.end()-1);
    _indexUnassignedList[k] = _indexUnassignedList[var->varNumber];
     *_indexUnassignedList[var->varNumber] = k;
    _unassignedVarList.pop_back();
}

inline void SatProblem::addUnassignedVar(Variable* var)
{
    _indexUnassignedList[var->varNumber] = _unassignedVarList.end();
    _unassignedVarList.push_back(var->varNumber);
}



//Version lente
/*
inline void SatProblem::deleteUnassignedVar(Variable* var)
{
    _unassignedVarPool.erase(var);
}

inline Literal SatProblem::chooseUnasignedVar()
{
    Variable* k = *(_unassignedVarPool.begin());
    deleteUnassignedVar(k);
    return Literal(k,true);
}

inline void SatProblem::addUnassignedVar(Variable* var)
{
    _unassignedVarPool.insert(var);
}
*/


#endif//SAT_SOLVER_HH
