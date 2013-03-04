#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <istream>

#include "Literal.hh"
#include "satSolver.hh"

#include "Clause.hh"
#include "BasicClause.hh"

#include "parser.hh"

typedef BasicClause UsedClause;




int main()
{
    SatProblem problem(std::cin);
    bool is_sat = problem.satisfiability();
    //Pour le bench
    if(is_sat)
    {
        std::cout << "s SATISFIABLE" << std::endl;
        const std::vector<varState>& assign = problem.getAssign();

        
        for(size_t k = 0; k < assign.size(); k++)
        {
            //std::cout << k+1 << "  =>  " << (assign[k] == TRUE ? "TRUE" : (assign[k]==FALSE) ? "FALSE" : "FREE") << std::endl;
            std::cout << "v ";
            if (assign[k] == FALSE)
                std::cout << "-";
            if (assign[k] == FREE)
                std::cout << "?";
            std::cout << k+1 << std::endl;
        }
        
    }
    else
    {
        std::cout << "s UNSATISFIABLE" << std::endl;
    }
}







/* Je ne comprends pas du tout ce que tu fais ici........ */
/* c'était pour avoir le type des attributs sous les yeux pendant que je code
   (j'ai oublié de le mettre en commentaire, dsl...

vector<varState> SatProblem::_varStates;
    vector<pair<set<Clause*>,set<Clause*> > > _variables;
    stack<pair<bool,Literal> > _stackCallback;
*/


SatProblem::SatProblem(std::istream& input)
{
    unsigned int nbrVar, nbrClauses;

    parserHeader(input, nbrVar, nbrClauses);

    // initialise les variables
    _varStates.resize(nbrVar, FREE);
    _variables.resize(nbrVar);

    // parse chaque clause du fichier
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        std::vector<Literal> list;
        parserListLit(input, list);

        // vérifie que les variables sont valides (d'indice entre 0 et nbrVar-1)
        for(unsigned int u = 0; u < list.size(); u++)
        {
            unsigned int var = list[u].var();
            if(var >= nbrVar)
            {
                std::cout <<"c Erreur de syntaxe dans l'entrée: "
                          <<"variable d'indice " << var+1 << " invalide "
                          <<"(l'indice doit être compris entre 1 et " << nbrVar << ")."
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
                    if(list[u].pos() == list[v].pos())
                    {
                        list[v] = list[list.size()-1];
                        list.pop_back();
                    }
                    else
                    {
                        trivial = true;
                        break;
                    }
                }
                else
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
                if(list[u].pos())
                {
                    _variables[var].first.insert(nclause);
                }
                else
                {
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

/** Ok mais pourquoi maintenir une liste des trucs supprimés ? Surtout que tu ajoutes que des trucs désalloués, non ? Au cas où la clause serait en double genre "1 -1" ? **/
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
            if(deleted.find(*it) == deleted.end())
            {
                delete *it;
                deleted.insert(*it);
            }
        }
        for(iter it = _variables[u].second.begin(); it != _variables[u].second.end(); it++)
        {
            if(deleted.find(*it) == deleted.end())
            {
                delete *it;
                deleted.insert(*it);
            }
        }
    }
}




Literal SatProblem::chooseUnasignedVar()
{
    unsigned int k = *_unassignedVarList.begin();
    _unassignedVarList.erase(*_unassignedVarList.begin());
    //while(k < _varStates.size() && _varStates[k] != FREE)
      //  k++;
    
    return Literal(k,true);
}




bool SatProblem::deduceFromSizeOne()
{
    unsigned int n = _varStates.size();
    std::stack<Literal> deductions;
    do
    {
        // on évite les déductions sur une variable déjà assignée
        while(!deductions.empty() && _varStates[deductions.top().var()]!=FREE)
        {
            // déduction contradictoire : on fait un callback
            if((_varStates[deductions.top().var()]==TRUE) != deductions.top().pos())
                return false;
            // sinon : la déduction correspond à l'état courant d'une variable : on l'ignore la déduction
            else
                deductions.pop();
        }

        // plusieurs choix :
        // soit une déduction, et on la fait (elle est valide : c'est ce dont on vient de s'assurer)
        // soit une nouvelle variable (si pas de déduction)
        // soit pas de déduction et toutes les variables sont déjà assignée : problème SATISFIABLE
        if(!deductions.empty())
        {
            Literal newAssign = deductions.top();
            deductions.pop();
            _unassignedVarList.erase(newAssign.var());
            _stackCallback.push( std::pair<bool,unsigned int>(false, newAssign.var()) );
            _varStates[newAssign.var()] = (newAssign.pos()) ? TRUE : FALSE;


            // propagation la nouvelle valeur
            std::set<Clause*>& cTrue = _variables[newAssign.var()].first;
            std::set<Clause*>& cFalse = _variables[newAssign.var()].second;
            std::set<Clause*>::iterator it;

            bool valInClause = newAssign.pos();
            for(it = cTrue.begin(); it != cFalse.end(); it++)
            {
                if(it == cTrue.end())
                {
                    it = cFalse.begin();
                    if(it == cFalse.end())
                        break;
                    valInClause = !valInClause;
                }

                if(valInClause)
                    (*it)->setLitTrue(newAssign);
                else
                    (*it)->setLitFalse(newAssign);

                if( (!(*it)->satisfied()) && (*it)->freeSize()==1 )
                    deductions.push( (*it)->chooseFree() );
                else if( (!(*it)->satisfied()) && ((*it)->freeSize()==0) )
                    return false;
            }
        }
        else
        {
            for (unsigned int var = 0; var < n; var++)
            {
                if (_variables[var].second.size() == 0)
                    deductions.push(Literal(var, true));
                else if (_variables[var].first.size() == 0)
                    deductions.push(Literal(var, false));
            }
            if (deductions.empty())
                break;
        }



        

    } while (!deductions.empty());

    return true;
}




bool SatProblem::satisfiability()
{
    const size_t n = _varStates.size();

    // remet à 0 l'état du solver
    while(! _stackCallback.empty() )
        _stackCallback.pop();
    for(size_t k = 0; k < n; k++)
    {
        _unassignedVarList.insert(k);
        _varStates[k] = FREE;
    }

    deduceFromSizeOne();

    std::stack<Literal> deductions;

    while(_stackCallback.size() < n || !deductions.empty())
    {
        // calcule la nouvelle valeur à assigner
        Literal newAssign(0,true);
        // on évite les déductions sur une variable déjà assignée
        while(!deductions.empty() && _varStates[deductions.top().var()]!=FREE)
        {
            // déduction contradictoire : on fait un callback
            if((_varStates[deductions.top().var()]==TRUE) != deductions.top().pos())
            {
                while(! deductions.empty())
                    deductions.pop();
                // on revient à la dernière supposition faite
                while(!_stackCallback.empty() && deductions.empty())
                {
                    // annule l'assignation de la dernière variable assignée
                    unsigned int varID = _stackCallback.top().second;
                    _unassignedVarList.insert(varID);
                    std::set<Clause*>::iterator it;
                    for(it = _variables[varID].first.begin(); it != _variables[varID].first.end(); it++)
                      (*it)->freeVar(varID);
                    for(it = _variables[varID].second.begin(); it != _variables[varID].second.end(); it++)
                      (*it)->freeVar(varID);
                    // si c'était une assignation libre, on sort en ajoutant le choix opposé comme déduction
                    if(_stackCallback.top().first)
                        deductions.push( Literal(varID, !(_varStates[varID]==TRUE)) );
                    _varStates[varID] = FREE;
                    _stackCallback.pop();
                }
                // si pas de déduction faite : problème INSATISFIABLE
                if(_stackCallback.empty() && deductions.empty())
                    return false;
            }
            // sinon : la déduction correspond à l'état courant d'une variable : on l'ignore la déduction
            else
                deductions.pop();
        }

        // plusieurs choix :
        // soit une déduction, et on la fait (elle est valide : c'est ce dont on vient de s'assurer)
        // soit une nouvelle variable (si pas de déduction)
        // soit pas de déduction et toutes les variables sont déjà assignée : problème SATISFIABLE
        if(!deductions.empty())
        {
            newAssign = deductions.top();
            deductions.pop();
            _unassignedVarList.erase(newAssign.var());
            _stackCallback.push( std::pair<bool,unsigned int>(false, newAssign.var()) );
        }
        else if(_stackCallback.size() < n)
        {
            newAssign = chooseUnasignedVar();
            _stackCallback.push( std::pair<bool,unsigned int>(true, newAssign.var()) );
        }
        else
            return true;
        
        _varStates[newAssign.var()] = (newAssign.pos()) ? TRUE : FALSE;


        // propagation la nouvelle valeur
        std::set<Clause*>& cTrue = _variables[newAssign.var()].first;
        std::set<Clause*>& cFalse = _variables[newAssign.var()].second;
        std::set<Clause*>::iterator it;

        bool is_error = false;
        bool valInClause = newAssign.pos();
        for(it = cTrue.begin(); it != cFalse.end(); it++)
        {
            if(it == cTrue.end())
            {
                it = cFalse.begin();
                if(it == cFalse.end())
                    break;
                valInClause = !valInClause;
            }
        
            if(valInClause)
                (*it)->setLitTrue(newAssign);
            else
                (*it)->setLitFalse(newAssign);

            if( (!(*it)->satisfied()) && (*it)->freeSize()==1 )
                deductions.push( (*it)->chooseFree() );
            else if( (!(*it)->satisfied()) && ((*it)->freeSize()==0) )
                // on arrive à une contadiction : on prend note, et le cas est géré à la sortie de la boucle
                // on fait donc la propagation de la variable en entier
                // ceci pour simplifier la propagation en arrière : on libère la variable de toutes les clauses où elle apparaît,
                // et non pas de toutes celles qu'on a parcouru avant d'arriver à la contradiction
                is_error = true;
        }

        // si une erreur : on fait le backtraking
        if(is_error)
        {
            // on déduit le contraire de newAssign pour profiter du code de backtracking déjà écrit du début de la boucle
            newAssign.invert();
            deductions.push( newAssign );
        }
    }

    return true;
}


