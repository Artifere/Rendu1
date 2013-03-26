#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
#include "UnassignedBucket.hh"
#include "parser.hh"


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
    // Pour accélérer les I/O
    std::ios_base::sync_with_stdio(false);

    // Parse le header dans l'entrée standard : récupère nombre de variables et de clauses
    unsigned int nbrVar, nbrClauses;
    parserHeader(std::cin, nbrVar, nbrClauses);

    // Initialise le problème en lisant l'entrée standard
    SatProblem problem(std::cin, nbrVar, nbrClauses);

    // Résoud le problème puis affiche la sortie correspondante
    #if VERBOSE == 0
    problem.satisfiability();
    #else
    if(problem.satisfiability())
    {
        std::cout << "s SATISFIABLE\n";
        const std::vector<std::pair<unsigned,varState> > assign = problem.getAssign();
        for(unsigned int k = 0; k < assign.size(); k++)
        {
            std::cout << "v ";
            if (assign[k].second == FALSE)
                std::cout << "-";
            if (assign[k].second == FREE)
                std::cout << "?";
            std::cout << assign[k].first << '\n';
        }
    }
    else
        std::cout << "s UNSATISFIABLE\n";
    #endif
}




SatProblem::SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses)
    : _unassignedVar(nbrVar)
{
    // Optionnel, rend l'initialisation un peu plus rapide
    _variables.reserve(nbrVar);
    _clauses.reserve(nbrClauses);

    // Initialise les variables
    for(unsigned k = 1; k <= nbrVar; k++)
        _variables.push_back(new Variable(k));

    // Parse chaque clause du fichier (et crée les liens entre variables et clauses)
    unsigned number = 0;
    std::vector<Literal> listClause;
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        listClause.clear();
        parserListLit(input, listClause, _variables);
        addClause(CALL_CONSTR(listClause));
        number++;
    }

    /* Initialise _unassignedVar */
    #if defined(INIT_SORT) && INIT_SORT
    /* commence par trier les variables : celles qui satisfont le plus de clauses sont placées en tête.
       C'est une heuristique faite pour améliorer le choix basique */
    std::sort(_variables.begin(), _variables.end(), varCompr);
    #endif
    std::vector<Variable*>::const_iterator it;
    for (it = _variables.begin(); it != _variables.end(); it++)
        _unassignedVar.addUnassigned(*it);

    // Affiche le nombre de fois que chaque variable apparaît dans une clause
    #if VERBOSE >= 5
    std::cout << "c Etat des variables à la fin du parsage : ";
    for (it = _variables.begin(); it != _variables.end(); it++)
    {
        (*it)->print_state();
        std::cout << " (" << (*it)->sizeLitTrue() << "x" << (*it)->sizeLitFalse() << "),  ";
    }
    std::cout << std::endl;
    #endif
}





SatProblem::~SatProblem()
{
    unsigned k;
    for(k = 0; k < _variables.size(); k++)
        delete _variables[k];
    for(k = 0; k < _clauses.size(); k++)
        delete _clauses[k];
}




/* Crée une clause, tout en vérifiant si elle n'est pas trivialement vraie/fausse, et déduit
   des assignations si la clause est de taille 1. Vérifie aussi que ces déductions ne sont pas contradictoires. */
void SatProblem::addClause(CONSTR_ARGS(list))
{
    // Supprime de list les doublons, et repère si la clause est trivialement vraie
    bool trivial = false; // ssi la clause  est trivialement vraie
    for (unsigned int u = 0; u < list.size() && !trivial; u++)
    {
        // Teste si x := list[u] est présent sous forme x ou !x
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
                    #if VERBOSE >= 2
                    print_debug();
                    std::cout << "Doublon repéré dans une clause (variable n°" << list[v].var()->varNumber << ")" << std::endl;
                    #endif
                    list[v] = list.back();
                    list.pop_back();
                }
            }
        }
    }

    /* On nee crée la clause que si elle n'est ni trivialement vraie, ni trivialement fausse.
       Dans ce cas, on associe la clause à toutes les variables qu'elle contient. */
    if(trivial)
    {
        #if VERBOSE >= 2
        print_debug();
        std::cout<<"Clause trivialement vraie lue. Elle est ignorée."<<std::endl;
        #endif
    }
    else if(list.size() == 0)
    {
        #if VERBOSE >= 2
        print_debug();
        std::cout<<"Clause trivialement fausse lue (clause vide)."<<std::endl;
        #endif
        
        #if VERBOSE > 0
        std::cout<<"s UNSATISFIABLE"<<std::endl;
        #endif
        exit(0);
    }
    else
    {
        // Si la clause est de taille 1, on fait une déduction
        if(list.size() == 1)
        {
            Literal& lit = *(list.begin());
            #if VERBOSE >= 2
            print_debug();
            std::cout<<"Clause à déduction immédiate ajoutée : "<<lit.var()->varNumber<<" (valeur "<<lit.pos()<<")"<<std::endl;
            #endif
            
            // On n'ajoute la déduction que si on ne l'a pas déjà faite
            if(lit.var()->_varState == FREE)
            {
                _deductions.push(lit);
                lit.var()->_varState = lit.pos() ? TRUE : FALSE;
            }
            else if((lit.var()->_varState == TRUE) != lit.pos())
            {
                #if VERBOSE >= 2
                print_debug();
                std::cout<<"Clause à déduction immédiate contradictoire avec une autre clause."<<std::endl;
                #endif
                #if VERBOSE > 0
                std::cout<<"s UNSATISFIABLE"<<std::endl;
                #endif
                exit(0);
            }
        }
        else // On crée la clause
        {
            Clause* nclause = new Clause(CALL_CONSTR(list));
            _clauses.push_back(nclause);
        }
    }
}




// Le coeur du solveur
bool SatProblem::satisfiability()
{
    // On continue l'exécution tant qu'on n'a pas assigné toutes les variables (ou que l'on a quitté la boucle à cause d'une contradiction)
    while(_stackBacktrack.size() < _variables.size())
    {
        Literal newAssign;
        if(_deductions.empty()) // On fait un pari
        {
            newAssign = _unassignedVar.chooseUnassigned();
            #if VERBOSE >= 3
            print_debug();
            std::cout<<"Assignation : ";
            newAssign.var()->print_state();
            std::cout << " (" << newAssign.var()->sizeLitTrue() << "x" << newAssign.var()->sizeLitFalse() << ") "
                      << " à " << newAssign.pos() << std::endl;
            #endif
            newAssign.var()->_varState = newAssign.pos() ? TRUE : FALSE;
            _stackBacktrack.push( std::pair<bool, Variable*>(true, newAssign.var()) );
        }
        else // On a une déduction
        {
            newAssign = _deductions.top();
            _deductions.pop();
            #if VERBOSE >= 3
            print_debug();
            std::cout<<"Assignation ";
            newAssign.var()->print_state();
            std::cout << " (" << newAssign.var()->sizeLitTrue() << "x" << newAssign.var()->sizeLitFalse() << ") ";
            std::cout<<"  (choix contraint)"<<std::endl;
            #endif
            // On enlève la variable de la liste des variables libres
            _unassignedVar.deleteUnassigned(newAssign.var());
            _stackBacktrack.push( std::pair<bool, Variable*>(false, newAssign.var()) );
        }
        #if VERBOSE >= 5
        print_debug();
        std::cout << "État des variables :   ";
        std::vector<Variable*>::const_iterator itDebug;
        for (itDebug = _variables.begin(); it_debug != _variables.end(); ++it_debug) {
            (*itDebug)->print_state();
            std::cout << " ";
        }
        std::cout<<std::endl;
        #endif

        // On propage la nouvelle assignation, et on regarde s'il y a eu une contradiction lors de la propagation
        const bool isError = newAssign.var()->propagateVariable(_deductions);

        // On fait le backtrack si besoin
        if(isError)
        {
            #if VERBOSE >= 3
            print_debug();
            std::cout<<"Backtrack"<<std::endl;
            #endif
            // On vide les déductions pas encore propagées dans les clauses
            while (!_deductions.empty())
            {
                #if VERBOSE >= 4
                print_debug();
                std::cout<<"suppression de la déduction ";
                _deductions.top().var()->print_state();
                std::cout << " (" << _deductions.top().var()->sizeLitTrue() << "x" << _deductions.top().var()->sizeLitFalse() << ")"
                          << "  (valeur déduite : " << _deductions.top().pos() << ")" << std::endl;
                #endif
                _deductions.top().var()->_varState = FREE;
                _deductions.pop();
            }
            // On revient au dernier choix libre fait
            do {
                // Si on n'a aucun choix libre, on renvoie faux (UNSAT)
                if (_stackBacktrack.empty())
                    return false;
                // Sinon, on libère la variable du haut de _stackCallback
                Variable * var = _stackBacktrack.top().second;
                #if VERBOSE >= 3
                print_debug();
                std::cout<<"Retour sur la valeur de la variable "<<var->varNumber<<std::endl;
                #endif
                // On libère la variable, dans toutes les clauses où elle était surveillée
                var->releaseVariable();
                
                /* Si c'était une assignation libre, on ajoute son contraire comme déduction.
                   Dans tous les cas, on la supprime du haut de _stackCallback. */
                if(!_stackBacktrack.top().first)
                    var->_varState = FREE;
                else
                {
                    const bool newVal = !(var->_varState == TRUE);
                    _deductions.push( Literal(var, newVal) );
                    var->_varState = newVal ? TRUE:FALSE;
                }
                // On vient de libérer une variable, on la rajoute donc à la liste des variables libres
                _unassignedVar.addUnassigned(var);
                _stackBacktrack.pop();
            } while (_deductions.empty());
            #if VERBOSE >= 4
            print_debug();
            std::cout<<"Fin du backtrack."<<std::endl;
            #endif
        }
    }
    return true;
}



