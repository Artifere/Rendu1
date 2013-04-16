#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>

#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
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
    // nécessaire pour que les itérateurs restent valides
    Variable::_vars.reserve(nbrVar+1);
    Variable::_endDeducted = Variable::_vars.begin();
    Variable::_endAssigned = Variable::_vars.begin();
    // Optionnel, rend l'initialisation un peu plus rapide
    _clauses.reserve(nbrClauses);
    
    // Initialise les variables
    //_varFantome = new Variable(0);
    for(unsigned k = 1; k <= nbrVar; k++)
        new Variable(k);
    //_varFantome->deductedFromFree(true, NULL);

    // Parse chaque clause du fichier (et crée les liens entre variables et clauses)
    std::vector<Literal> listClause;
    for(unsigned int k = 0; k < nbrClauses; k++)
    {
        listClause.clear();
        parserListLit(input, listClause, Variable::_vars);
        addClause(listClause, NULL);
    }

    // éventuel tri initial des variables libres
    #if defined(INIT_SORT) && (INIT_SORT == 1)
    Variable::sortFreeVars();
    #endif

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
    for(k = 0; k < _absoluteAssigned.size(); k++)
        delete _absoluteAssigned[k];
    for(k = 0; k < _clauses.size(); k++)
        delete _clauses[k];
}




/* Crée une clause, tout en vérifiant si elle n'est pas trivialement vraie/fausse, et déduit
   des assignations si la clause est de taille 1. Vérifie aussi que ces déductions ne sont pas contradictoires. */
void SatProblem::addClause(std::vector<Literal>& list, Variable* firstTrue)
{
    static unsigned int number = 0;
    number ++;
    #if VERBOSE > 1
    print_debug();
    std::cout << "Ajout de clause : ";
    for(unsigned kDebug = 0; kDebug < list.size(); kDebug++)
        std::cout << list[kDebug].var()->varNumber << "." << list[kDebug].pos() << ", ";
    std::cout << std::endl;
    #endif
    // supprime les variable de _absoluteAssigned
    std::vector<Variable*>::const_iterator assigned;
    for (assigned = _absoluteAssigned.begin(); assigned != _absoluteAssigned.end(); ++assigned)
    {
       std::vector<Literal>::iterator read = list.begin();
       while (read != list.end())
       {
           if (read->var() == *assigned)
               if (read->pos() == (*assigned)->_varState)
               {
                   // on vide la clause car elle est trivialement vraie
                   list.clear();
                   break;
               }
               else
               {
                   // on supprime le litéral de la clause car il est faux
                   list.pop_back();
                   *read = *list.end();
               }
           else
               read ++;
        }
    }
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
    if(trivial)
    {
        #if VERBOSE >= 2
        print_debug();
        std::cout<<"Clause trivialement vraie lue. Elle est ignorée."<<std::endl;
        #endif
    }
    else if(list.size() == 0)// || (list.size() == 1 && list[0].var() == _varFantome))
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
        // Si la clause est de taille 1, on fait une déduction absolue :
        // on supprime de l'ensemble du problème la clause et partout ou elle apparaît
        if(list.size() == 1)
        {
            #if VERBOSE >= 2
            print_debug();
            std::cout<<"Clause à déduction immédiate ajoutée : "<<list[0].var()->varNumber<<" (valeur "<<list[0].pos()<<")"<<std::endl;
            #endif
            absoluteAssign(list[0]);
        }
        else // On crée la clause
        {
            Clause* nclause = new Clause(CALL_CONSTR(list), firstTrue);
            _clauses.push_back(nclause);
        }
    }
}


void SatProblem::absoluteAssign(Literal lit)
{
    if (! lit.var()->isFree())
        std::cout << "Absolute assign : Ceci ne devrait pas arriver : le litéral doit être libre" << std::endl;
    // TODO : changer ça por quelque chose de plus intelligent
                // on récupère une variable différente de mergedLits[0]
                Variable * diff = Variable::_vars[(lit.var() == Variable::_vars[0]) ? 1 : 0];
                // on ajoute deux clauses : l'une avec diff, l'autre avec non(diff)
                std::vector<Literal> liste(1, lit);
                liste.push_back(Literal(diff,true));
                addClause(liste, lit.var());
                liste.pop_back();
                liste.push_back(Literal(diff, false));
                addClause(liste, lit.var());
    /*
    // TODO : supprimmer la variable de l'ensemble du problème
    lit.var()->_varState = lit.pos();
    // déplace la variable* de Variable::_vars vers _absoluteAssigned
    lit.var()->moveToLastVar();
    Variable::_vars.pop_back();
    _absoluteAssigned.push_back(lit.var());
    // parcourt toutes les clauses, pour supprimmer la variable recherchée
    */

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
            Variable::chooseFromFree();
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
        const Clause* isError = newAssign->assignedFromDeducted();

        // On fait le backtrack si besoin
        if(isError != NULL)
        {
            #if VERBOSE >= 2
            print_debug();
            std::cout<<"Backtrack"<<std::endl;
            #endif
            // Si on n'a aucun choix libre, on renvoie faux (UNSAT)
            if (_stackBacktrack.empty())
                return false;

            // sinon : on apprend de nos erreurs et on revient sur le dernier paris fait
            std::pair<std::vector<Literal>,Literal> newDeduct(resolve(isError));

            // On revient au dernier choix libre fait
            std::vector<Variable*>::iterator it, lastChoice = _stackBacktrack.back();
            _stackBacktrack.pop_back();
            do {
                Variable * var = * (-- Variable::_endAssigned);
                #if VERBOSE >= 2
                print_debug();
                std::cout<<"Retour sur la valeur de la variable "<<var->varNumber<<std::endl;
                #endif
                // On libère la variable, des clauses où elle était surveillée
                var->deductedFromAssigned();
            } while (Variable::_endAssigned > lastChoice);
            Variable::_endDeducted = lastChoice;
            
            #if VERBOSE >= 2
            print_debug();
            std::cout<<"Fin du backtrack."<<std::endl;
            print_debug();
            std::cout << "État des variables :   ";
            std::vector<Variable*>::const_iterator itDebug;
            for (itDebug = Variable::_vars.begin(); itDebug != Variable::_vars.end(); ++ itDebug) {
                (*itDebug)->print_state();
                std::cout << " ";
            }
            std::cout<<std::endl;
            std::cout << "number assigned : " << (Variable::_endAssigned - Variable::_vars.begin()) << std::endl;
            std::cout << "number deducted : " << (Variable::_endDeducted - Variable::_endAssigned) << std::endl;
            std::cout << "deduct from free : " << newDeduct.second.var()->varNumber << std::endl;
            #endif
            
            // on ajoute ce qu'on a appris comme déduction
            addClause(newDeduct.first, newDeduct.second.var());
            newDeduct.second.var()->deductedFromFree(newDeduct.second.pos(), _clauses.back());
            
            #if VERBOSE >= 2
            print_debug();
            std::cout<<"Fin du backtrack."<<std::endl;
        print_debug();
        std::cout << "État des variables :   ";
        for (itDebug = Variable::_vars.begin(); itDebug != Variable::_vars.end(); ++ itDebug) {
            (*itDebug)->print_state();
            std::cout << " ";
        }
        std::cout<<std::endl;
        std::cout << "number assigned : " << (Variable::_endAssigned - Variable::_vars.begin()) << std::endl;
        std::cout << "number deducted : " << (Variable::_endDeducted - Variable::_endAssigned) << std::endl;
            #endif
        }
    }
    return true;
}



inline bool litCompVar(const Literal& lit1, const Literal& lit2)
{
    return lit1.var() < lit2.var();
}



std::pair<std::vector<Literal>,Literal>  SatProblem::resolve(const Clause *conflictClause)
{
    #if VERBOSE > 1
        print_debug();
        std::cout << "État des variables :   ";
        std::vector<Variable*>::const_iterator itDebug;
        for (itDebug = Variable::_vars.begin(); itDebug != Variable::_vars.end(); ++ itDebug) {
            (*itDebug)->print_state();
            std::cout << " ";
        }
        std::cout<<std::endl;
        std::cout << "Pari courant : " << (*_stackBacktrack.back())->varNumber << std::endl;
        std::cout << "End deducted : " << (*Variable::_endDeducted)->varNumber << std::endl;
        std::cout << "Contradiction depuis la clause " << conflictClause->_number << std::endl;
    #endif

    std::vector<Literal> mergedLits(conflictClause->getLiterals());
    
    
    
    sort(mergedLits.begin(), mergedLits.end(), litCompVar);
    
    Literal youngest;
    while (true)
    {
        unsigned nbFromCurBet = 0;
        youngest = * mergedLits.begin();
        for (std::vector<Literal>::iterator it = mergedLits.begin();it != mergedLits.end(); ++it)
        {
            if (it->var()->isFromCurBet(_stackBacktrack.back()))
            {
                #if VERBOSE > 1
                std::cout << "variable trouvée : " << it->var()->varNumber << std::endl;
                #endif
                nbFromCurBet++;
                if (youngest.var()->isOlder(it->var()))
                {
                    youngest = * it;
                }
            }
        }
        
        if(nbFromCurBet == 0 || youngest.var() == NULL) {
            std::cout << "ceci ne devrait pas arriver" << std::endl;
            break;
        }
        if(nbFromCurBet == 1)
            break;

        
        Clause *deductedFrom = youngest.var()->getOriginClause();
        std::vector<Literal> toMerge = deductedFrom->getLiterals();
        #if VERBOSE > 1
        std::cout << "merge la clause " << deductedFrom->_number << " qui a permis de déduire la variable " << youngest.var()->varNumber << std::endl;
        #endif
        sort(toMerge.begin(), toMerge.end(), litCompVar);
        
        std::vector<Literal> res(mergedLits.size()+toMerge.size());
        std::vector<Literal>::iterator resIt;
        resIt = std::set_union(mergedLits.begin(), mergedLits.end(), toMerge.begin(), toMerge.end(), res.begin(), litCompVar);
        //resIt = std::remove(res.begin(), resIt, Literal(youngestVar, true));
        //resIt = std::remove(res.begin(), resIt, Literal(youngestVar, false));
        res.resize(resIt-res.begin());
        res.erase(std::lower_bound(res.begin(), res.end(), youngest, litCompVar));
        std::swap(mergedLits, res);
    }

    return std::pair<std::vector<Literal>,Literal>(mergedLits, youngest);
    /*// TODO plus tard : utiliser une autre methode pour gérér les clauses de taille 1
    if(mergedLits.size() == 1)
    {
        // on récupère une variable différente de mergedLits[0]
        Variable * diff = Variable::_vars[(mergedLits[0].var() == Variable::_vars[0]) ? 1 : 0];
        // on ajoute deux clauses : l'une avec diff, l'autre avec non(diff)
        mergedLits.push_back(Literal(diff, true));
        addClause(mergedLits, youngest.var());
        mergedLits.pop_back();
        mergedLits.push_back(Literal(diff, false));
        addClause(mergedLits, youngest.var());
    }
    else
    {
        addClause(mergedLits, youngest.var());
    }
    return youngest;*/
}




