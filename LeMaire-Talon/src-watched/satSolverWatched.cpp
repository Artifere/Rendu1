#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <istream>

#include "Literal.hh"
#include "satSolverWatched.hh"

#include "Clause.hh"
#include "BasicClauseWatched.hh"

#include "parser.hh"






int main()
{
    SatProblem problem(std::cin);

    bool is_sat = problem.satisfiability();
    //Pour le bench
//#ifdef RELEASE
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
//#endif
}






SatProblem::SatProblem(std::istream& input)
{
    unsigned int nbrVar, nbrClauses;

    parserHeader(input, nbrVar, nbrClauses);

    // initialise les variables
    _varStates.resize(nbrVar, FREE);
    _variables.resize(nbrVar);

    _indexUnassignedList.resize(nbrVar);
    _unassignedVarList.reserve(nbrVar);
    for (unsigned int var = 0; var < nbrVar; var++)
       addUnassignedVar(var);


    // parse chaque clause du fichier
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        std::vector<Literal> list;
        parserListLit(input, list, nbrVar);
        addClause(list);
    }

    #ifndef RELEASE
    for(unsigned int var = 0; var < nbrVar; var++)
    {
        std::cout << "c  la variable " << (var+1) << " apparaît " << _variables[var].first.size() << " + " << _variables[var].second.size() << " fois." << std::endl;
    }
    #endif
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
    // on doit désalouer toutes les clauses (allouées dans SatProblem())
    // (le reste se détruit tout seul)

    for (std::set<StockedClause*>::const_iterator it = _clausesList.begin(); it != _clausesList.end(); ++it)
        delete *it;

    /*std::set<StockedClause*> deleted;
    for(unsigned int u = 0; u < _variables.size(); u++)
    {
        for(iter it = _variables[u].first.begin(); it != _variables[u].first.end(); ++it)
        {
            if(deleted.find(*it) == deleted.end())
            {
                delete *it;
                deleted.insert(*it);
            }
        }
        for(iter it = _variables[u].second.begin(); it != _variables[u].second.end(); ++it)
        {
            if(deleted.find(*it) == deleted.end())
            {
                delete *it;
                deleted.insert(*it);
            }
        }
    }*/
}



void SatProblem::addClause(std::vector<Literal>& list)
{
    // supprime de list les doublons, et repère si trivialement vraie
    bool trivial = false; // ssi clause trivialement vraie
    for(unsigned int u = 0; u < list.size() && !trivial; u++)
    {
        // test si x=list[u] est présent sous forme x ou !x
        unsigned int v = u+1;
        while(v < list.size() && !trivial)
        {
            if(list[u].var() != list[v].var())
                v++;
            else
            {
                if(list[u].pos() != list[v].pos())
                    trivial = true;
                else
                {
                    list[v] = list[list.size()-1];
                    list.pop_back();
                }
            }
        }
    }
    // ne crée la clause que si elle n'est ni trivialement vraie, ni trivialement fausse
    // dans ce cas, associe la clause à toutes les variables concernées
    if(!trivial && list.size()!=0)
    {
        // on passe par des pointeurs pour garder la structure d'objet :
        // UsedClause hérite de StockedClause, donc UsedClause* passe pour StockedClause*
        // alors que UsedClause ne passe pas pour StockedClause à priori
        StockedClause * nclause = new UsedClause(list);
        if (_clausesList.find(nclause) != _clausesList.end())
            return;
        else
            _clausesList.insert(nclause);

        // si une seule variable dans la clause : on déduit cette variable
        if(list.size() == 1)
        {
            Literal& lit = *(list.begin());
            // on ajoute la déduction que si on ne l'a pas déjà faite
            // (si c'est une contradiction, on la détectera lors de la résolution de toutes façons)
            if(_varStates[lit.var()] == FREE)
            {
                _deductions.push(lit);
                _varStates[lit.var()] = lit.pos() ? TRUE : FALSE;
            }
        }
        for(unsigned int u = 0; u < 2; u++)
        {
            unsigned int var = list[u].var();
            if(list[u].pos())
                _variables[var].first.insert(nclause);
            else
                _variables[var].second.insert(nclause);
        }
    }
    // si clause triviallement fausse : on l'ignore, et on affiche un warning
    else if(list.size() == 0)
    {
#if RELEASE
        std::cout <<"c Attention : clause trivialement fausse lue (clause vide). "
                  <<"Elle est ignorée, même si le problème avec cette clause est "
                  <<"instatisfiable (la résolution continuera sans la clause)."
                  <<std::endl;
#endif
    }
}


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
            deleteUnassignedVar(newAssign.var());
            _stackCallback.push( std::pair<bool, unsigned int>(false, newAssign.var()) );
            _deductions.pop();
        }
        bool is_error = propagateVariable(newAssign);

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

                // si c'était une assignation libre, on ajoute son contraire comme déduction.
                // dans tous les cas, on la supprime du haut de _stackCallback
                if(_stackCallback.top().first)
                {
                    bool newVal = !(_varStates[varID]==TRUE);
                    _deductions.push( Literal(varID, newVal) );
                    _varStates[varID] = newVal ? TRUE:FALSE;
                    addUnassignedVar(varID);
                }
                else
                {
                    _varStates[varID] = FREE;
                    addUnassignedVar(varID);
                }
                _stackCallback.pop();
            } while(_deductions.empty());
        }
    }
    return true;
}





bool SatProblem::propagateVariable(const Literal& lit)
{
    std::set<StockedClause*>& cTrue  = lit.pos() ? _variables[lit.var()].first : _variables[lit.var()].second;
    std::set<StockedClause*>& cFalse = lit.pos() ? _variables[lit.var()].second : _variables[lit.var()].first;

    bool is_error = false;

    std::set<StockedClause*>::const_iterator it;
    
    //for (it = cTrue.begin(); it != cTrue.end(); ++it)
        // on passe la clause à true : pas besoin de tester une déduction où une contradiction
    //    (*it)->setLitTrue(lit, *this);
    
    // on sépare en deux pour faire encore quelques tests de moins si il y a une erreure
    // (comme je sais que tu t'inquiète de quelques tests ;)
    for (it = cFalse.begin(); (!is_error) && (it != cFalse.end()); ++it)
    {
        (*it)->setLitFalse(lit, *this);
        // si clause contradictoire : on renvoie une erreur
        bool notsat = !(*it)->satisfied(*this);
        if (notsat && (*it)->freeSize(*this) == 0)
            is_error = true;
        // sinon, si pas déduction, ne rien faire
        // et si déduction : on teste si elle n'est pas contradictoire
        else if (notsat && (*it)->freeSize(*this) == 1)
        {
            Literal deduct = (*it)->chooseFree(*this);
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
    // on finit la propagation (si une erreur à eu lieu) mais sans essayer de trouver d'autres déductions
    for (; it != cFalse.end(); ++it)
        (*it)->setLitFalse(lit, *this);

    for (it = cTrue.begin(); it != cTrue.end(); ++it)
        (*it)->setLitTrue(lit, *this);
    
    
    while (!_toRemoveL.empty())
    {
        Literal curL;
        StockedClause *curC;

        curL = _toRemoveL.top();
        curC = _toChangeC.top();

        if (!curL.pos())
            _variables[curL.var()].first.erase(curC);
        else
            _variables[curL.var()].second.erase(curC);

        curL = _toInsertL.top();
        if (curL.pos())
            _variables[curL.var()].first.insert(curC);
        else
            _variables[curL.var()].second.insert(curC);

        _toChangeC.pop();
        _toInsertL.pop();
        _toRemoveL.pop();

    }
    return is_error;
}




