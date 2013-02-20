#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>

typedef
enum etat_var {
  TRUE, FALSE, FREE
} etat_var;


class SatProblem {
  protected:
    vector<etat_var> _etats_var;
    vector<pair<set<Clause*>,set<Clause*>> _variables;
    stack<pair<bool,Literal>> _stack_callback;
    
  public:
    SatProblem(unsigned int nbr_var, unsigned int nbr_clauses, const istream& input);
    ~SatProblem();
    
    inline const vector<etat_var>& get_assign() const { return _etats_var; }
    bool satisfiability();
};




#endif//SAT_SOLVER_HH
