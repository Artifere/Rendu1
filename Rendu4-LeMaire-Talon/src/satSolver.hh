#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <stack>
#include <utility>
#include <istream>

#include "Clause.hh"
#include "Literal.hh"


#define EXIT_SATISFIABLE 10
#define EXIT_UNSATISFIABLE 20


class SatProblem
{
protected:
    const unsigned _nbrVars;
    std::vector<Clause*> _clauses;
    // pile des indices des choix contraints
    std::vector<std::vector<Variable*>::iterator> _stackBacktrack;
    
    
public:
    // input doit être un flux listant toutes les clauses : liste de nombres entre -nbrVar et nbrVar, se terminant par un 0
    SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses);
    ~SatProblem();
    inline const std::vector<Variable*>::iterator getLastBetIterator(void) const {return _stackBacktrack.back();}
    inline const std::vector<std::pair<unsigned,bool> > getAssign(void) const;
    
    std::pair<std::vector<Literal>,Literal> resolve(Variable *conflictVar) const;

    bool simplify(std::vector<Literal>& list) const;
    
    void addClause(const std::vector<Literal>& litsList, Literal lit = Literal(NULL,true));

    bool satisfiability(void);

    void interact(const std::pair<std::vector<Literal>,Literal>& learned, Variable* conflit);

    void createConflictGraph(Variable *conflictClause) const;
    
    friend std::ostream& operator<< (std::ostream& out, const SatProblem&);
};





inline const std::vector<std::pair<unsigned, bool> > SatProblem::getAssign(void) const
{
    typedef std::pair<unsigned,bool> returnType;
    std::vector<returnType> res(_nbrVars);

    for (unsigned k = 0; k < _nbrVars; k++)
        res[k] = returnType(Variable::_vars[k]->varNumber, Variable::_vars[k]->_varState);
    return res;
}


        




#endif // SAT_SOLVER_HH defined
