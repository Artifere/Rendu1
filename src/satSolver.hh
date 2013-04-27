#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <stack>
#include <utility>
#include <istream>

#include "Clause.hh"
#include "Literal.hh"



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
    
    std::pair<std::vector<Literal>,Literal> resolve(Variable *conflictVar) const;

    bool simplify(std::vector<Literal>& list) const;
    
    void addClause(const std::vector<Literal>& litsList, Literal lit = Literal(NULL,true));

    bool satisfiability(void);

    void interact(const std::pair<std::vector<Literal>,Literal>& learned, Variable* conflit);

    void createConflictGraph(Variable *conflictClause) const;
    
    friend std::ostream& operator<< (std::ostream& out, const SatProblem&);
};






        




#endif // SAT_SOLVER_HH defined
