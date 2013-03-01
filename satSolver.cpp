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
        //bool varValides = true; //Inutile ????
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




Literal SatProblem::chooseUnasignedVar() const
{
    unsigned int k = 0;
    while(k < _varStates.size() && _varStates[k] != FREE)
        k++;
    return Literal(k,true);
}




bool SatProblem::satisfiability()
{
    const size_t n = _varStates.size();

    // remet à 0 l'état du solver
    while(! _stackCallback.empty() )
        _stackCallback.pop();
    for(size_t k = 0; k < n; k++)
        _varStates[k] = FREE;

    std::stack<Literal> deductions;

    while(_stackCallback.size() < n && deductions.empty())
    {


        // calcule la nouvelle valeur à assigner
        Literal newAssign(0,true);

        // on évite les déductions sur une variable déjà assignée
        while( (!deductions.empty()) && _varStates[deductions.top().var()]!=FREE )
        {
            // déduction contradictoire : on fait un callback
            if( (_varStates[deductions.top().var()]==TRUE) != deductions.top().pos() )
            {
                std::cout << "Callback needed" << std::endl;
                // clear all deductions
                /** Pourquoi toutes ? Seulement celles contradictoires jusqu'à la première qui va... **/
                /** parce qu'elles sont basées sur une supposition erronnées,
                 et qu'on ne sait pas si elles seront encore vraies sans cette supposition.
                 C'est juste les assignations pas encore faites qu'on supprime... **/
                while(! deductions.empty())
                    deductions.pop();
                // on revient à la dernière déduction faite
                while( (! _stackCallback.empty()) )
                {
                    // annule l'assignation de la variable
                    unsigned int varID = _stackCallback.top().second;
                    std::set<Clause*>::iterator it;
                    for(it = _variables[varID].first.begin(); it != _variables[varID].first.end(); it++)
                      (*it)->freeVar(varID);
                    for(it = _variables[varID].second.begin(); it != _variables[varID].second.end(); it++)
                      (*it)->freeVar(varID);
                    // si c'était une assignation libre, on sort en ajoutant le choix opposé comme déduction
                    if(_stackCallback.top().first)
                    {
                        deductions.push(Literal(varID, !(_varStates[varID]==TRUE)));
                        _varStates[varID] = FREE;
                        _stackCallback.pop();
                        break;
                    }
                    _varStates[varID] = FREE;
                    _stackCallback.pop();
                }
                // si pas de déduction faite : problème INSATISFIABLE
                if(_stackCallback.empty())
                    return false;
            }
            // sinon : la déduction correspond à l'état courant d'une variable :
            // on ignore la déduction
            else
            {
                deductions.pop();
            }
        }

        // on assigne soit une nouvelle variable (si pas de déduction), soit le contraire de la dernière déduction
        // soit pas de déduction et toutes les variables sont déjà assignée : problème SATISFIABLE
        if(! deductions.empty())
        {
            newAssign = deductions.top();
            deductions.pop();
            _stackCallback.push( std::pair<bool,unsigned int>(false, newAssign.var()) );
        }
        else if(_stackCallback.size() >= n)
        {
            return true;
        }
        else
        {
            newAssign = chooseUnasignedVar();
            _stackCallback.push( std::pair<bool,unsigned int>(true, newAssign.var()) );
        }
        _varStates[newAssign.var()] = (newAssign.pos()) ? TRUE : FALSE;


        // propagation la nouvelle valeur
        std::set<Clause*>& cTrue = _variables[newAssign.var()].first;
        std::set<Clause*>& cFalse = _variables[newAssign.var()].second;
        std::set<Clause*>::iterator it;

        bool is_error = false;
        bool lit_true = newAssign.pos(); // retient si on explore les clauses dans lesquelles le literal est vrai ou faux (/!\ : différent du fait que le litéral soit x ou !x : ce n'est pas si on est entrain de parcourir cTrue ou cFalse, mais bien si la clause qu'on explore passe à vrai, ou si on ne fait que supprimmer un litéral de la clause)

        for(it = cTrue.begin(); it != cFalse.end(); it++)
        {
            // passe des clauses cTrue à cFalse
            if( it == cTrue.end() )
            {
                it = cFalse.begin();
                //On teste si cFalse est vide
                if (it == cFalse.end())
                    break;
                lit_true = ! lit_true;
            }

            // assigne la variable dans la clause
            if(lit_true)
                (*it)->setLitTrue(newAssign);
            else
                (*it)->setLitFalse(newAssign);

            // on déduit une nouvelle valeur
            if( (!(*it)->satisfied()) && (*it)->freeSize()==1 )
            {
                deductions.push( (*it)->chooseFree() );
            }
            else
                // on arrive à une contadiction : on prend note, et le cas est géré à la sortie de la boucle
                // on fait donc la propagation de la variable en entier
                // ceci pour simplifier la propagation en arrière : on libère la variable de toutes les clauses,
                // et non pas de toutes celles qu'on a parcouru avant d'arriver à la contradiction
                if( (!(*it)->satisfied()) && ((*it)->freeSize()==0) )
                {
                    is_error = true;
                }
        }

        // si une erreur : on fait le backtraking
        if(is_error)
        {
            // on déduit le contraire de newAssign pour profiter du code de backtracking déjà écrit du début de la clause
            newAssign.invert();
            deductions.push( newAssign );
        }
    }

    return true;
}


