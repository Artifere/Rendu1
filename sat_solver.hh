#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <utility>
#include "Clause.hh"
using namespace std;

typedef
enum etat_var {
  TRUE, FALSE, FREE
} etat_var;


class SatProblem {
  protected:
    vector<etat_var> _etats_var;
    vector<pair<set<Clause*>, set<Clause*> > > _variables;
    stack<pair<bool,Literal> > _stack_callback;
    
  public:
    SatProblem(/*unsigned int nbr_var, unsigned int nbr_clauses, */const istream& input); //Me semble que ça doit être ça...
    ~SatProblem();
    
    inline const vector<etat_var>& get_assign() const { return _etats_var; }
    bool satisfiability();
};
    
    
    




#endif//SAT_SOLVER_HH
