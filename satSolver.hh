#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <stack>
#include <utility>
#include <istream>

#include "Clause.hh"
#include "Literal.hh"
//#include "UnassignedBucket.hh"




class SatProblem
{
protected:
    const unsigned _nbrVars;
    std::vector<Clause*> _clauses;    
    // pile des indices des choix contraints
    std::vector<std::vector<Variable*>::iterator> _stackBacktrack;
    
public:
    SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses);
    ~SatProblem();
    inline const std::vector<Variable*>::iterator getLastBetIterator(void) const {return _stackBacktrack.back();}
    inline const std::vector<std::pair<unsigned,bool> > getAssign(void) const;
    
    void  resolve(Clause *conflictClause);

    void addClause(CONSTR_ARGS(list), Variable *firstTrue);
    bool satisfiability();
};




// Appelée à la fin du programme dans le cas où le problème est satisfiable
inline const std::vector<std::pair<unsigned, bool> > SatProblem::getAssign(void) const
{
    typedef std::pair<unsigned,bool> return_type;
    std::vector<return_type> res(_nbrVars);

    for (unsigned k = 0; k < _nbrVars; k++)
        res[k] = return_type(Variable::_vars[k]->varNumber, Variable::_vars[k]->_varState);
    return res;
}
        




#endif // SAT_SOLVER_HH defined
