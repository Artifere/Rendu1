#ifndef SAT_SOLVER_HH
#define SAT_SOLVER_HH

#include <vector>
#include <set>
#include <stack>
#include <utility>
#include "Clause.hh"
#include <istream>
typedef
enum etat_var {
  TRUE, FALSE, FREE
} etat_var;


class SatProblem {
  protected:
    std::vector<etat_var> _etats_var;
    std::vector<std::pair<std::set<Clause*>, std::set<Clause*> > > _variables;
    //std::stack<std::pair<bool,Literal> > _stack_callback;
    std::stack<std::pair<bool,unsigned int> > _stack_callback; //Je pense que c'est mieux. le bool c'est pour dire si on peut changer la valeur de la variable ou si c'était un choix nécessaire
                                                               //l'autre paramètre c'est le numéro de la variable
    
    unsigned int chooseUnasignedVar() const;
    
  public:
    SatProblem(std::istream& input);
    ~SatProblem();
    
    inline const std::vector<etat_var>& get_assign() const
    {
      return _etats_var;
    }
    bool satisfiability();
};

    
    




#endif//SAT_SOLVER_HH
