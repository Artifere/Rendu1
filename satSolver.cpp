#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>


#include "Literal.hh"
#include "satSolver.hh"

#include "Clause.hh"
#include "BasicClause.hh"

#include "parser.hh"

using namespace std;
typedef BasicClause UsedClause;


/*int main() {
  cout << "hello world !" << endl;
  return 0;
}*/







/* Je ne comprends pas du tout ce que tu fais ici........ */
/* c'était pour avoir le type des attributs sous les yeux pendant que je code
   (j'ai oublié de le mettre en commentaire, dsl...
   
vector<etat_var> SatProblem::_etats_var;
    vector<pair<set<Clause*>,set<Clause*> > > _variables;
    stack<pair<bool,Literal> > _stack_callback;
*/


SatProblem::SatProblem(std::istream& input)
{
  unsigned int nbr_var, nbr_clauses;
  
  parser_header(input, nbr_var, nbr_clauses);
  
  // initialise les variables
  _etats_var.resize(nbr_var, FREE);
  _variables.resize(nbr_var);

  // parse chaque clause du fichier
  for(unsigned int k = 0; k < nbr_clauses; k++)
  {
    std::vector<Literal> list;
    parser_list_lit(input, list);
    
    // vérifie que les variables sont valides (d'indice entre 0 et nbr_var-1)
    bool var_valides = true;
    for(unsigned int u = 0; u < list.size(); u++)
    {
      unsigned int var = list[u].var();
      if(var < 1 || var > nbr_var)
      {
        std::cout <<"c Erreur de syntaxe dans l'entrée: "
                  <<"variable d'indice " << var << " invalide "
                  <<"(l'indice doit être compris entre 1 et " << nbr_var << ")."
                  <<std::endl;
        // le programme continu en ignorant la variable
        std::cout <<"c La variable est ignorée." << std::endl;
        list[u--] = list[list.size()-1];
        list.pop_back();
      }
    }
    
    // supprime de list les doublons, et repère si trivialement vraie
    bool trivial = false; // ssi clause trivialement vraie
    for(unsigned int u = 0; u < list.size() && !trivial; u++)
    {
      // test si x=list[u] est présent sous forme x ou !x
      unsigned int v = u+1;
      while(v < list.size())
      {
        if(list[u].var() == list[v].var())
        {
          if(list[u].pos() == list[v].pos()) {
            list[v] = list[list.size()-1];
            list.pop_back();
          } else {
            trivial = true;
            break;
          }
        } else
        {
          v++;
        }
      }
    }
    // ne crée la clause que si elle n'est pas trivialement vraie
    // dans ce cas, associe la clause à toutes les variables concernées
    if(!trivial)
    {
      // on passe par des pointeurs pour garder la structure d'objet :
      // UsedClause hérite de Clause, donc UsedClause* passe pour Clause*
      // alors que UsedClause ne passe pas pour Clause
      Clause * nclause = new UsedClause(list);
      for(unsigned int u = 0; u < list.size(); u++)
      {
        unsigned int var = list[u].var();
        if(list[u].pos()) {
          _variables[var].first.insert(nclause);
        } else {
          _variables[var].second.insert(nclause);
        }
      }
    }
  }
  
  // ajouter un test pour savoir si le fichier est vide ?
  // (pour repérer les erreurs dans le fichier, comme minisat)

}



/* on pourait s'en passer parce que l'objet SatProblem vit tant que le main s'exécute,
 * et est détruit à la fin (donc tout est automatiquement déaslloué quand on quitte le main)
 * Ça reste plus propre de tout désallouer à la main je trouve
 */
SatProblem::~SatProblem()
{
  typedef std::set<Clause*>::iterator iter;
  // on doit désalouer toutes les clauses (allouées dans SatProblem())
  // (le reste se détruit tout seul)
  std::set<Clause*> deleted;
  for(unsigned int u = 0; u < _variables.size(); u++)
  {
    for(iter it = _variables[u].first.begin(); it != _variables[u].first.end(); it++)
    {
      iter f = deleted.find(*it);
      if(f != deleted.end()) {
        delete *f;
        deleted.insert(*it);
      }
    }
    for(iter it = _variables[u].second.begin(); it != _variables[u].second.end(); it++)
    {
      iter f = deleted.find(*it);
      if(f != deleted.end()) {
        delete *f;
        deleted.insert(*it);
      }
    }
  }
}





