#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <istream>

#include "Literal.hh"
#include "Clause.hh"
#include "BasicClause.hh"




typedef
enum etat_var {
  TRUE, FALSE, FREE
} etat_var;


class SatProblem {
  protected:
    std::vector<etat_var> _etats_var;
    std::vector< std::pair<std::set<Clause*>, std::set<Clause*> > > _variables;
    std::stack<std::pair<bool,Literal> > _stack_callback;
    
  public:
    SatProblem(std::istream& input);
    ~SatProblem();
    
    inline const std::vector<etat_var>& get_assign() const { return _etats_var; }
    bool satisfiability();
};




#endif//SAT_SOLVER_HH
