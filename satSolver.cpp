#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>


#include "Literal.hh"
#include "sat_solver.hh"

#include "Clause.hh"
#include "BasicClause.hh"

#include "parser.hh"


typedef BasicClause UsedClause;


/*int main() {
  cout << "hello world !" << endl;
  return 0;
}*/







/* Je ne comprends pas du tout ce que tu fais ici........ */
vector<etat_var> SatProblem::_etats_var;
    vector<pair<set<Clause*>,set<Clause*> > > _variables;
    stack<pair<bool,Literal> > _stack_callback;
    

SatProblem::SatProblem(const istream& input) {
  unsigned int nbr_var, nbr_clauses;
  
  parser_header(input, nbr_var, nbr_clauses);
  
  _etats_var.reserve(nbr_var);
  _variables.reserve(nbr_var);
  
  std::vector<Literal> list;
  for(unsigned int k = 0; k < nbr_clauses; k++) {
    parser_list_lit(input, list);
    for(unsigned int u = 0; u < list.size(); u++) {
    for(unsigned int v = u+1; v < list.size(); v++) {  //Remplacé l par u
      if(list[u].var() == list[v].var()) {
        
  
    //~SatProblem(); // Utile ?
    
    inline const vector<etat_var>& get_assign() const
    {
      return _etats_var;
    }
    bool satisfiability();
};





