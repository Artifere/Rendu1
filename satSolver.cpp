#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
//#include "UnassignedBucket.hh"
#include "parser.hh"
#include <algorithm>


#if VERBOSE > 1
#define print_debug()  \
    std::cout<<"c "; \
    {  \
        for(unsigned k = 0; k < _stackBacktrack.size(); k++) \
            std::cout << "|  ";  \
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
        const std::vector<std::pair<unsigned,bool> > assign = problem.getAssign();
        for(unsigned int k = 0; k < assign.size(); k++)
        {
            std::cout << "v ";
            if (! assign[k].second)
                std::cout << "-";
            std::cout << assign[k].first << '\n';
        }
    }
    else
        std::cout << "s UNSATISFIABLE\n";
    #endif
}




SatProblem::SatProblem(std::istream& input, const unsigned int nbrVar, const unsigned int nbrClauses)
    : _nbrVars(nbrVar)
{
    // nécessaire pour que les itérateus restent valides
    Variable::_vars.reserve(nbrVar);
    Variable::_endDeducted = Variable::_vars.begin();
    Variable::_endAssigned = Variable::_vars.begin();
    // Optionnel, rend l'initialisation un peu plus rapide
    _clauses.reserve(nbrClauses);
    
    // Initialise les variables
    for(unsigned k = 1; k <= nbrVar; k++)
        new Variable(k);

    // Parse chaque clause du fichier (et crée les liens entre variables et clauses)
    unsigned number = 1;
    std::vector<Literal> listClause;
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        listClause.clear();
        parserListLit(input, listClause, Variable::_vars);
        addClause(CALL_CONSTR(listClause));
        number++;
    }

    // Affiche le nombre de fois que chaque variable apparaît dans une clause
    #if VERBOSE >= 5
    std::cout << "c Etat des variables à la fin du parsage : ";
    std::vector<Variable*>::iterator it;
    for (it = Variable::_vars.begin(); it != Variable::_vars.end(); it++)
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
    for(k = 0; k < Variable::_vars.size(); k++)
        delete Variable::_vars[k];
    for(k = 0; k < _clauses.size(); k++)
        delete _clauses[k];
}




/* Crée une clause, tout en vérifiant si elle n'est pas trivialement vraie/fausse, et déduit
   des assignations si la clause est de taille 1. Vérifie aussi que ces déductions ne sont pas contradictoires. */
void SatProblem::addClause(CONSTR_ARGS(list))
{
    #if VERBOSE > 6
    print_debug();
    std::cout << "Ajout de clause : ";
    for(unsigned kDebug = 0; kDebug < list.size(); kDebug++)
        std::cout << list[kDebug].var()->varNumber << "." << list[kDebug].pos() << ", ";
    std::cout << std::endl;
    #endif
    // supprime les doublons (O(n.log n))
    std::sort(list.begin(), list.end());
    list.resize(std::unique(list.begin(), list.end()) - list.begin());
    // teste si la clause est trivialement vraie :
    bool trivial = false;
    for(unsigned k = 1; k < list.size(); k++)
    {
        if (list[k-1].var() == list[k].var()) {
            trivial = true;
            break;
        }
    }
/*
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
                list[v] = list.back();
                list.pop_back();
            }
        }
    }
    #if VERBOSE > 6
    print_debug();
    std::cout << "   apres simplification : ";
    for(unsigned kDebug = 0; kDebug < list.size(); kDebug++)
        std::cout << list[kDebug].var()->varNumber << "." << list[kDebug].pos() << ", ";
    std::cout << std::endl;
    #endif
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
            if(lit.var()->isFree())
                lit.var()->deductedFromFree(lit.pos());
            else if(lit.var()->_varState != lit.pos())
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
    while (Variable::_endAssigned != Variable::_vars.end())
    {
        // si pas de déduction : on doit faire un pari
        if(Variable::_endAssigned == Variable::_endDeducted)
        {
            // choisit une variable libre, qu'on ajoute aux déductions
            Variable::choseFromFree_DLIS();
            // ajoute un choix libre pour le backtrack
            _stackBacktrack.push_back(Variable::_endAssigned);
        }
        Variable * newAssign = * (Variable::_endAssigned ++);
        #if VERBOSE >= 3
        print_debug();
        std::cout<<"Assignation : ";
        newAssign->print_state();
        std::cout << " (" << newAssign->sizeLitTrue() << "x" << newAssign->sizeLitFalse() << ") "
                  << " à " << (bool)newAssign->_varState << std::endl;
        #endif

        #if VERBOSE >= 6
        print_debug();
        std::cout << "État des variables :   ";
        std::vector<Variable*>::const_iterator itDebug;
        for (itDebug = Variable::_vars.begin(); itDebug != Variable::_vars.end(); ++ itDebug) {
            (*itDebug)->print_state();
            std::cout << " ";
        }
        std::cout<<std::endl;
        #endif

        // On propage la nouvelle assignation, et on regarde s'il y a eu une contradiction lors de la propagation
        const bool isError = newAssign->assignedFromDeducted();

        // On fait le backtrack si besoin
        if(isError)
        {
            #if VERBOSE >= 3
            print_debug();
            std::cout<<"Backtrack"<<std::endl;
            #endif
            // Si on n'a aucun choix libre, on renvoie faux (UNSAT)
            if (_stackBacktrack.empty())
                return false;
            
            // On revient au dernier choix libre fait
            std::vector<Variable*>::iterator it, lastChoice = _stackBacktrack.back();
            _stackBacktrack.pop_back();
            do {
                Variable * var = * (-- Variable::_endAssigned);
                #if VERBOSE >= 3
                print_debug();
                std::cout<<"Retour sur la valeur de la variable "<<var->varNumber<<std::endl;
                #endif
                // On libère la variable, des clauses où elle était surveillée
                var->deductedFromAssigned();
            } while (Variable::_endAssigned > lastChoice);
            
            // on ajoute son contraire comme déduction
            (* lastChoice)->_varState = ! (* lastChoice)->_varState;
            Variable::_endDeducted = lastChoice + 1;
            
            #if VERBOSE >= 4
            print_debug();
            std::cout<<"Fin du backtrack."<<std::endl;
            #endif
        }
    }
    return true;
}



