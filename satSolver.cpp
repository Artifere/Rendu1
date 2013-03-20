#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
#include "UnassignedBucket.hh"

#include "parser.hh"

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cstdlib> // pour exit()
#include <string>


#if VERBOSE > 1
#define print_debug()  \
    std::cout<<"c "; \
    {  \
        std::stack<std::pair<bool,Variable*> > tmp_debug_print;  \
        while(! _stackBacktrack.empty()) {  \
            tmp_debug_print.push(_stackBacktrack.top());  \
            _stackBacktrack.pop();  \
        }  \
        while(! tmp_debug_print.empty()) {  \
            if(tmp_debug_print.top().first)  \
                std::cout << "|  ";  \
            _stackBacktrack.push(tmp_debug_print.top());  \
            tmp_debug_print.pop();  \
        }  \
    }
#else
#define print_debug()  
#endif


int main()
{
    std::ios_base::sync_with_stdio(false);
    
    unsigned int nbrVar, nbrClauses;
    parserHeader(std::cin, nbrVar, nbrClauses);

    SatProblem problem(std::cin, nbrVar, nbrClauses);

    bool is_sat = problem.satisfiability();
    #if VERBOSE > 0
    if(is_sat)
    {
        std::cout << "s SATISFIABLE" << std::endl;
        const std::vector<std::pair<unsigned,varState> > assign = problem.getAssign();
        for(size_t k = 0; k < assign.size(); k++)
        {
            std::cout << "v ";
            if (assign[k].second == FALSE)
                std::cout << "-";
            if (assign[k].second == FREE)
                std::cout << "?";
            std::cout << assign[k].first << std::endl;
        }
    }
    else
        std::cout << "s UNSATISFIABLE" << std::endl;
    #endif
}





static inline bool varCompr(const Variable* v1, const Variable* v2)
{
    return std::max(v1->sizeLitTrue(), v1->sizeLitFalse()) < std::max(v2->sizeLitTrue(), v2->sizeLitFalse());
    //return v1->sizeLitTrue() + v1->sizeLitFalse() < v2->sizeLitTrue() + v2->sizeLitFalse();
    //eturn v2->sizeLitTrue() + v2->sizeLitFalse() < v1->sizeLitTrue() + v1->sizeLitFalse(); // 5.5s pour BasicClauseWatched sur unsat-41
}


SatProblem::SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses)
    : _unassignedVar(nbrVar)
{
    // optionnel. rend l'initialisation un peu plus rapide
    _variables.reserve(nbrVar);
    _clauses.reserve(nbrClauses);
    
    // initialise les variables
    for(unsigned k = 1; k <= nbrVar; k++)
        _variables.push_back(new Variable(k));
    
    // parse chaque clause du fichier (et crée les liens entre variables et clauses)
    unsigned number = 0;
    std::vector<Literal> listClause;
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        listClause.clear();
        parserListLit(input, listClause, _variables);
        addClause(CALL_CONSTR(listClause));
        number++;
    }
    
    // initialise _unassignedVar
    // commence par trier les variables (heuristique faite pour améliorer le choix de unassignedVar dans le cas non rand)
    std::sort(_variables.begin(), _variables.end(), varCompr);
    std::vector<Variable*>::const_iterator it;
    for (it = _variables.begin(); it != _variables.end(); it++)
        _unassignedVar.addUnassigned(*it);
    
    // affiche le nombre de fois que chaque variable apparaît dans une clause
    #if VERBOSE > 2
    std::cout << "c Etat des variables à la fin du parsage : ";
    for (it = _variables.begin(); it != _variables.end(); it++)
    {
        (*it)->print_state();
        std::cout << " (" << (*it)->sizeLitTrue() << "x" << (*it)->sizeLitFalse() << "),  ";
    }
    std::cout << std::endl;
    #endif
    
    // ajouter un test pour savoir si le fichier est vide ?
    // (pour repérer les erreurs dans le fichier, comme minisat)
}





SatProblem::~SatProblem()
{
    unsigned k;
    for(k = 0; k < _variables.size(); k++)
        delete _variables[k];
    for(k = 0; k < _clauses.size(); k++)
        delete _clauses[k];
}





void SatProblem::addClause(CONSTR_ARGS(list))
{
    // supprime de list les doublons, et repère si trivialement vraie
    bool trivial = false; // ssi clause trivialement vraie
    for (unsigned int u = 0; u < list.size() && !trivial; u++)
    {
        // test si x=list[u] est présent sous forme x ou !x
        unsigned int v = u+1;
        while (v < list.size())
        {
            if (list[u].var() != list[v].var())
                v++;
            else
            {
                if (list[u].pos() != list[v].pos())
                {
                    trivial = true;
                    break;
                }
                else
                {
                    #if VERBOSE > 3
                    print_debug();
                    std::cout << "Doublon repéré dans une clause (variable n°" << list[v].var()->varNumber << ")" << std::endl;
                    #endif
                    list[v] = list.back();
                    list.pop_back();
                }
            }
        }
    }
    // ne crée la clause que si elle n'est ni trivialement vraie, ni trivialement fausse
    // dans ce cas, associe la clause à toutes les variables concernées
    if(trivial)
    {
        #if VERBOSE > 1
        print_debug();
        std::cout<<"Clause trivialement vraie lue. Elle est ignorée."<<std::endl;
        #endif
    }
    else if(list.size() == 0)
    {
        #if VERBOSE > 1
        print_debug();
        std::cout<<"c Clause trivialement fausse lue (clause vide)."<<std::endl;
        #endif
        #if VERBOSE > 0
        std::cout<<"s UNSATISFIABLE"<<std::endl;
        #endif
        exit(0);
    }
    else
    {
        // si une seule variable dans la clause : on déduit cette variable
        if(list.size() == 1)
        {
            Literal& lit = *(list.begin());
            #if VERBOSE > 2
            print_debug();
            std::cout<<"Clause à déduction immédiate ajoutée : "<<lit.var()->varNumber<<" (valeur "<<lit.pos()<<")"<<std::endl;
            #endif
            // on ajoute la déduction que si on ne l'a pas déjà faite
            if(lit.var()->_varState == FREE)
            {
                _deductions.push(lit);
                lit.var()->_varState = lit.pos() ? TRUE : FALSE;
            }
            else if((lit.var()->_varState == TRUE) != lit.pos())
            {
                #if VERBOSE > 1
                print_debug();
                std::cout<<"Clause à déduction immédiate contradictoire avec une autre clause."<<std::endl;
                #endif
                #if VERBOSE > 0
                std::cout<<"s UNSATISFIABLE"<<std::endl;
                #endif
                exit(0);
            }
        }
        else
        {
            // on passe par des pointeurs pour garder la structure d'objet :
            // UsedClause hérite de Clause, donc UsedClause* passe pour Clause*
            // alors que UsedClause ne passe pas pour Clause à priori
            // (et on perd l'interet de la surcharge avec la conversion de UsedClause vers Clause)
            Clause* nclause = new Clause(CALL_CONSTR(list));
            _clauses.push_back(nclause);
        }
    }
}





bool SatProblem::satisfiability()
{
    while(_stackBacktrack.size() < _variables.size())
    {
        // calculer la nouvelle valeur
        Literal newAssign;
        if(_deductions.empty())
        {
            newAssign = _unassignedVar.chooseRAND();
            //newAssign = _unassignedVar.chooseUnassigned();
            #if VERBOSE > 1
            print_debug();
            std::cout<<"Assignation : ";
            newAssign.var()->print_state();
            std::cout << " (" << newAssign.var()->sizeLitTrue() << "x" << newAssign.var()->sizeLitFalse() << ") "
                      << " à " << newAssign.pos() << std::endl;
            #endif
            newAssign.var()->_varState = newAssign.pos() ? TRUE : FALSE;
            _stackBacktrack.push( std::pair<bool, Variable*>(true, newAssign.var()) );
        }
        else
        {
            newAssign = _deductions.top();
            _deductions.pop();
            #if VERBOSE > 1
            print_debug();
            std::cout<<"Assignation ";
            newAssign.var()->print_state();
            std::cout << " (" << newAssign.var()->sizeLitTrue() << "x" << newAssign.var()->sizeLitFalse() << ") ";
            std::cout<<"  (choix contraint)"<<std::endl;
            #endif
            _unassignedVar.deleteUnassigned(newAssign.var());
            _stackBacktrack.push( std::pair<bool, Variable*>(false, newAssign.var()) );
        }
        #if VERBOSE > 5
        print_debug();
        std::cout << "État des variables :   ";
        std::vector<Variable*>::const_iterator it_debug;
        for (it_debug = _variables.begin(); it_debug != _variables.end(); ++it_debug) {
            (*it_debug)->print_state();
            std::cout << " ";
        }
        std::cout<<std::endl;
        #endif
        
        const bool isError = newAssign.var()->propagateVariable(_deductions);

        // on fait le callback si besoin
        if(isError)
        {
            #if VERBOSE > 1
            print_debug();
            std::cout<<"Backtrack"<<std::endl;
            #endif
            // vide les déductions pas encore propagées dans les clauses
            while (!_deductions.empty())
            {
                #if VERBOSE > 4
                print_debug();
                std::cout<<"suppression de la déduction ";
                _deductions.top().var()->print_state();
                std::cout << " (" << _deductions.top().var()->sizeLitTrue() << "x" << _deductions.top().var()->sizeLitFalse() << ")"
                          << "  (valeur déduite : " << _deductions.top().pos() << ")" << std::endl;
                #endif
                _deductions.top().var()->_varState = FREE;
                _deductions.pop();
            }
            // on revient au dernier choix libre fait
            do {
                // si pas de choix libre fait, on renvoie faux (UNSAT)
                if (_stackBacktrack.empty())
                    return false;
                // sinon, on libère la variable du haut de _stackCallback
                Variable * var = _stackBacktrack.top().second;
                #if VERBOSE > 2
                print_debug();
                std::cout<<"Retour sur la valeur de la variable "<<var->varNumber<<std::endl;
                #endif
                var->releaseVariable();
                // si c'était une assignation libre, on ajoute son contraire comme déduction.
                // dans tous les cas, on la supprime du haut de _stackCallback
                if(!_stackBacktrack.top().first)
                    var->_varState = FREE;
                else
                {
                    const bool newVal = !(var->_varState == TRUE);
                    _deductions.push( Literal(var, newVal) );
                    var->_varState = newVal ? TRUE:FALSE;
                }
                _unassignedVar.addUnassigned(var);
                _stackBacktrack.pop();
            } while (_deductions.empty());
            #if VERBOSE > 4
            print_debug();
            std::cout<<"Fin du backtrack."<<std::endl;
            #endif
        }
    }
    return true;
}



