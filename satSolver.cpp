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
/** si je ne le faisais pas, chaque clause serait désallouées autant de fois qu'elle a de variables (c-à-d à chaque fois que je la verait depuis une variable) **/
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
    unsigned int k = 0;// *_unassignedVarList.begin();
    //_unassignedVarList.erase(*_unassignedVarList.begin());
    while(k < _varStates.size() && _varStates[k] != FREE)
      k++;

    return Literal(k,true);
}




bool SatProblem::deduceFromSizeOne()
{
    unsigned int n = _varStates.size();
    std::stack<Literal> deductions;

    for (unsigned int var = 0; var < n; var++)
    {
        for (std::set<Clause*>::iterator it = _variables[var].first.begin(); it != _variables[var].first.end(); ++it)
            if ((*it)->freeSize() == 1)
                deductions.push(Literal(var, true));
    }

    for (unsigned int var = 0; var < n; var++)
    {
        for (std::set<Clause*>::iterator it = _variables[var].second.begin(); it != _variables[var].second.end(); ++it)
            if ((*it)->freeSize() == 1)
                deductions.push(Literal(var, false));
    }


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





/* version un peu plus propre de la fonction, qui devenait bordélique

 deductions : ensemble de valeurs à propager. d'intersection vide avec celles déjà assignées, et aucune contradictions entre les déductions
 on a pas besoin de deducedState en réalité : chaque deduction à sa valeur déjà écrite dans _varStates
 
 le gros changement depuis la fonction précédente, c'est qu'on fait le callback à la fin de la bocle plutôt qu'au début (parce que c'est là qu'on à dedécté une contradiction : on ne fait que réutiliser la variable is_error pour savoir si on doit faire le callback ou non)
 
 
 tant qu'on a pas assigné toutes les variables
   si deductions = vide
     choisir un literal libre et une valeur booléenne 
   sinon choisir une déduction
   
   propager la nouvelle valeur :
   pour chaque clause dont on peut tirer un litéral :
     si litéral libre (test d'après deductStates)
       ajouter le littéral dans deductStates
     sinon, si contredit une valeur de deductState
       contradiction
     sinon (déductions déjà faite précédemment)
       ne rien faire
   
   si contradiction :
   faire le callback :
     vider deductions (et remettre à FREE les valeurs des variables déduites dans _varStates)
     faire
       si plus de choix (_stackCallback = vide)
         renvoyer faux (UNSAT)
       sinon :
         on concidère la variale du haut de _stackCallback :
         si cette variable est un choix libre
           ajouter son contraire à deductions
         la liberer
     tant que deductions n'est pas vide
 fin tant que
 renvoyer vrai (SAT)
*/

/*
 Pour deduceFromSizeOne, je pense qu'on doit traiter séparément les deux types d'optimisations :

 simpification des clauses ne contenant qu'une variable :
   on peut faire un prétraîtement quand on crée le problème :
   si une clause ne contient qu'une seule variable,
     on assigne cette variable à la bonne valeur (et on n'ajoute même pas la clause)
 
 si une variable n'apparaît que sous la forme x ou !x :
   on assigne la variable à la bonne valeur (et on supprime les clauses correspondantes)
   ce traitement est à faire avant l'appel à satisfiability()
 */
bool SatProblem::satisfiability()
{
    while(_stackCallback.size() < _varStates.size() || !_deductions.empty())
    {
    
        // calculer la nouvelle valeur
        Literal newAssign(-1, true);
        if(_deductions.empty())
        {
            newAssign = chooseUnasignedVar();
            _stackCallback.push( std::pair<bool, unsigned int>(true, newAssign.var()) );
            _varStates[newAssign.var()] = newAssign.pos()?TRUE:FALSE;
        }
        else
        {
            newAssign = _deductions.top();
            _stackCallback.push( std::pair<bool, unsigned int>(false, newAssign.var()) );
            _deductions.pop();
        }

        // propager la nouvelle valeur
        bool is_error = false;
        bool valInClause = newAssign.pos();
        std::set<Clause*>& cTrue = _variables[newAssign.var()].first;
        std::set<Clause*>& cFalse = _variables[newAssign.var()].second;
        std::set<Clause*>::iterator it;
        for(it = cTrue.begin(); it != cFalse.end(); it++)
        {
            // propagation "moche" (en une seule boucle) à travers les clauses contenant x et !x
            if(it == cTrue.end())
            {
                it = cFalse.begin();
                if(it == cFalse.end())
                    break;
                valInClause = !valInClause;
            }
            
            // on passe la clause à true : pas besoin de tester une déduction où une contradiction
            if(valInClause)
                (*it)->setLitTrue(newAssign);
            // on enlève une des variables de la clause : on peut alors avoir des nouvelles déductions ou des des contradictions
            else
            {
                (*it)->setLitFalse(newAssign);
                
                //if(is_error || (*it)->satisfied() || (*it)->freeSize() != 1)
                //    is_error = is_error || (!(*it)->satisfied() && (*it)->freeSize()==0);
                //else
                // contradiction : taille passe à 0
                if(is_error || (!(*it)->satisfied() && (*it)->freeSize() == 0))
                    is_error = true;
                // sinon, si pas déduction, ne rien faire
                // et si déduction : on teste si elle n'est pas contradictoire
                else if( !(*it)->satisfied() && (*it)->freeSize() == 1)
                {
                    Literal deduct = (*it)->chooseFree();
                    // si la déduction concerne une nouvelle variable, on l'ajoute
                    if(_varStates[deduct.var()] == FREE)
                    {
                        _deductions.push(deduct);
                        _varStates[deduct.var()] = deduct.pos() ? TRUE:FALSE;
                    }
                    // sinon, si déduction déjà faite, on ne fait rien
                    // et si déduction contraire déjà faite, contradiction
                    else if(deduct.pos() != (_varStates[deduct.var()] == TRUE))
                        is_error = true;
                }
            }
        }
        
        
        // on fait le callback si besoin
        if(is_error)
        {
            // vide les déductions pas encore propagées dans les clauses
            while(!_deductions.empty())
            {
                _varStates[_deductions.top().var()] = FREE;
                _deductions.pop();
            }
            // on revient au dernier choix libre fait
            do {
                // si pas de choix libre fait, on renvoie faux (UNSAT)
                if(_stackCallback.empty())
                    return false;
                // sinon, on libère la variable du haut de _stackCallback
                unsigned varID = _stackCallback.top().second;
                std::set<Clause*>::iterator it;
                for(it = _variables[varID].first.begin(); it != _variables[varID].first.end(); it++)
                    (*it)->freeVar(varID);
                for(it = _variables[varID].second.begin(); it != _variables[varID].second.end(); it++)
                    (*it)->freeVar(varID);
                // si c'était une assignation libre, on ajoute son contraire comme déduction.
                // dans tous les cas, on la supprime du haut de _stackCallback
                if(_stackCallback.top().first)
                {
                    bool newVal = !(_varStates[varID]==TRUE);
                    _deductions.push( Literal(varID, newVal) );
                    _varStates[varID] = newVal ? TRUE:FALSE;
                }
                else
                {
                    _varStates[varID] = FREE;
                }
                _stackCallback.pop();
            } while(_deductions.empty());
        }
    }
    return true;
}


#if FALSE // backup
bool SatProblem::satisfiability()
{
    const size_t n = _varStates.size();

    _deducedState.resize(n, FREE);
    // remet à 0 l'état du solver
    while(! _stackCallback.empty() )
        _stackCallback.pop();
    for(size_t k = 0; k < n; k++)
    {
        _unassignedVarList.insert(k);
        _varStates[k] = FREE;
    }

    std::stack<Literal> deductions;

    //if(!deduceFromSizeOne())
    //  return false;

    while(_stackCallback.size() < n || !deductions.empty())
    {
        // calcule la nouvelle valeur à assigner
        Literal newAssign(0,true);
        /*std::stack<Literal> tmp = deductions;
        while (!tmp.empty())
        {
            std::cout << tmp.top().var() << (tmp.top().pos()? "true":"false") << std::endl;
            tmp.pop();
        }
        std::cout << "--------------\n";*/
        // on évite les déductions sur une variable déjà assignée
        if(!deductions.empty() && _varStates[deductions.top().var()]!=FREE)
        {
            // déduction contradictoire : on fait un callback
            if((_varStates[deductions.top().var()]==TRUE) != deductions.top().pos())
            {
                while(! deductions.empty())
                {
                    //if (_varStates[deductions.top().var()] == FREE)
                        _deducedState[deductions.top().var()] = FREE;
                    deductions.pop();
                }
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
                    {
                        bool newVal = !(_varStates[varID]==TRUE);
                        deductions.push( Literal(varID, newVal) );
                        _deducedState[varID] = newVal ? TRUE : FALSE;
                    }
                    else
                        _deducedState[varID] = FREE;

                    _varStates[varID] = FREE;
                    _stackCallback.pop();
                }
                // si pas de déduction faite : problème INSATISFIABLE
                if(_stackCallback.empty() && deductions.empty())
                    return false;
            }
            // sinon : la déduction correspond à l'état courant d'une variable : on l'ignore la déduction
            else
            {
                //_deducedState[deductions.top().var()] = FREE;
                deductions.pop();
            }
        }

        // plusieurs choix :
        // soit une déduction, et on la fait (elle est valide : c'est ce dont on vient de s'assurer)
        // soit une nouvelle variable (si pas de déduction)
        // soit pas de déduction et toutes les variables sont déjà assignée : problème SATISFIABLE
        if(!deductions.empty())
        {
            newAssign = deductions.top();
            //_deducedState[newAssign.var()] = (newAssign.;
            deductions.pop();
            _unassignedVarList.erase(newAssign.var());
            _stackCallback.push( std::pair<bool,unsigned int>(false, newAssign.var()) );
        }
        else if(_stackCallback.size() < n)
        {
            newAssign = chooseUnasignedVar();
            _stackCallback.push( std::pair<bool,unsigned int>(true, newAssign.var()) );
            _deducedState[newAssign.var()] = (newAssign.pos() ? TRUE:FALSE);
        }
        else
            return true;

        _varStates[newAssign.var()] = _deducedState[newAssign.var()];//idem (newAssign.pos()) ? TRUE : FALSE;


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

            if (!is_error)
            {
                if( (!(*it)->satisfied()) && (*it)->freeSize()==1 )
                {
                    Literal l = (*it)->chooseFree();
                    if ((!l.pos() && _deducedState[l.var()] == TRUE) || (l.pos() && _deducedState[l.var()] == FALSE))
                        is_error = true;
                    else if (_deducedState[l.var()] == FREE)
                    {
                        _deducedState[l.var()] = (l.pos() ? TRUE:FALSE);
                        deductions.push(l);
                    }
                }
                else if( (!(*it)->satisfied()) && ((*it)->freeSize()==0) )
                    // on arrive à une contadiction : on prend note, et le cas est géré à la sortie de la boucle
                    // on fait donc la propagation de la variable en entier
                    // ceci pour simplifier la propagation en arrière : on libère la variable de toutes les clauses où elle apparaît,
                    // et non pas de toutes celles qu'on a parcouru avant d'arriver à la contradiction
                    is_error = true;
            }
        }

        // si une erreur : on fait le backtraking
        if(is_error)
        {
            // on déduit le contraire de newAssign pour profiter du code de backtracking déjà écrit du début de la boucle
            newAssign.invert();
            deductions.push( newAssign );
            //_deducedState[newAssign.var()] = (newAssign.pos() ? TRUE : FALSE);
        }
        /* Bugge je ne sais pas pourquoi
           else
           {
           for (unsigned int var = 0; var < n; var++)
           {
           if (_varStates[var] == FREE)
           {
           bool isTrue = true;
           for (std::set<Clause*>::iterator it = _variables[var].first.begin(); isTrue && it != _variables[var].first.end(); ++it)
           isTrue = (*it)->satisfied();
           if (isTrue)
           deductions.push(Literal(var, false));
           else
           {
           isTrue = true;
           for (std::set<Clause*>::iterator it = _variables[var].second.begin(); isTrue && it != _variables[var].second.end(); ++it)
           isTrue = (*it)->satisfied();
           if (isTrue)
           deductions.push(Literal(var, true));
           }
           }
           }
           }*/

    }

    return true;
}
#endif

