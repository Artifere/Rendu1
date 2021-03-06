#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include "satSolver.hh"

#include "Variable.hh"
#include "Literal.hh"
#include "Clause.hh"
#include "parser.hh"
#include <queue>

#if VERBOSE > 1
#define print_debug()  \
    std::cout<<"c "; \
    {  \
        for(unsigned k = 0; k < _stackBacktrack.size(); k++) \
            std::cout << "| ";  \
    }
#else
#define print_debug()
#endif


#if VERBOSE > 0
#define print_vars() \
    for (std::vector<Variable*>::const_iterator itDebug = Variable::_vars.begin(); itDebug != Variable::_vars.end(); ++ itDebug) \
    { \
        if (itDebug == Variable::_endAssigned || itDebug == Variable::_endDeducted) \
            std::cout << " | "; \
        else std::cout << "   "; \
        (*itDebug)->print_state(); \
    } \
    std::cout << std::endl
#endif


int main(int argc, char *argv[])
{
    // Pour accélérer les I/O
    //std::ios_base::sync_with_stdio(false);

    std::ifstream input;
	input.open (argv[1], std::ios::in);
    if (!input.is_open())
        exit(1);
    // Parse le header dans l'entrée standard : récupère nombre de variables et de clauses
    unsigned int nbrVar, nbrClauses;
    parserHeader(input, nbrVar, nbrClauses);
    // Initialise le problème en lisant l'entrée standard
    SatProblem problem(input, nbrVar, nbrClauses);
    input.close();
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



// modifie la liste en entrée pour enlever les doublons, repérer si trivialement fausse ou vraie :
// renvoie true ssi trivialement vraie
// sinon, list est sans doublons (vide si clause trivialement fausse)
bool SatProblem::simplify(std::vector<Literal>& list) const
{
    // supprime les doublons (O(n.log n))
    std::sort(list.begin(), list.end());
    list.resize(std::unique(list.begin(), list.end()) - list.begin());

    const unsigned listSize = list.size();
    // teste si la clause est trivialement vraie
    for(unsigned k = 1; k < listSize; k++)
    {
        if (list[k-1].var() == list[k].var())
            return true;
    }
    
    return false;
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
    for(unsigned k = 1; k <= nbrVar; k++)
        new Variable(k);

    // Initialise les clauses :
    // Parse chaque clause du fichier (et crée les liens entre variables et clauses)
    std::vector<Literal> listClause;
    for(unsigned k = 0; k < nbrClauses; k++)
    {
        listClause.clear();
        parserListLit(input, listClause, Variable::_vars);
        // clause trivialement vraie
        if (simplify(listClause))
        {
            #if VERBOSE > 3
            std::cout << "c Clause trivialement vraie lue (" << k+1 << "eme lue). Elle est ignorée." << std::endl;
            #endif
        }
        else 
        // on fait un push_back même si on n'a pas réelement créé de clause :
        // cela permet d'avoir un compte réel du nombre de clauses concidérées
            addClause(listClause);
    }

    // éventuel tri initial des variables libres
    #if defined(INIT_SORT) && (INIT_SORT == 1)
    Variable::sortFreeVars();
    #endif

    // Affiche l'état de chaque variable, le nombre de liens avec les clauses
    #if VERBOSE >= 5
    std::cout << "c Etat des variables à la fin du parsage : ";
    print_vars();
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




void SatProblem::addClause(const std::vector<Literal>& litsList, Literal lit)
{
    static unsigned number = 0;
    number ++;
    Clause * newC = NULL;

    const unsigned litsListSize = litsList.size();
    // clause triialement fausse
    if(litsListSize == 0)
    {
        #if VERBOSE > 0
        std::cout<<"c Clause trivialement fausse lue (clause vide)."<<std::endl;
        std::cout<<"s UNSATISFIABLE"<<std::endl;
        #endif
        exit(20);
    }
    // clause de taille 1 : on ne la crée pas, mais on déduit la valeur de la variable
    else if(litsListSize == 1)
    {
        #if VERBOSE > 3
        std::cout << "c Clause à déduction immédiate lue : " << litsList[0].var()->varNumber << '.' << litsList[0].pos() << std::endl;
        #endif
        if (litsList[0].var()->isFree())
        {
            litsList[0].var()->deductedFromFree(litsList[0].pos(), NULL);            
        }
        if(!litsList[0].var()->isFree() && litsList[0].var()->_varState != litsList[0].pos())
        {
            #if VERBOSE > 0
            std::cout<<"s UNSATISFIABLE"<<std::endl;
            #endif
            exit(20);
        }
    }
    // sinon : ajout réel de la clause, dans ce cas on on déduit un litéral de la clause, et l'autre
    else if(lit.var() == NULL)
    {
        newC = new Clause(litsList, number);
        _clauses.push_back(newC);
    }
    else
    {
        newC = new Clause(litsList, number, lit.var());
        _clauses.push_back(newC);
        lit.var()->deductedFromFree(lit.pos(), newC);            
    }
}





// Le coeur du solveur
bool SatProblem::satisfiability()
{
    #if INTERACT
        static int nbLeftBeforePrompt = 0;
    #endif
    // On continue l'exécution tant qu'on n'a pas assigné toutes les variables (ou que l'on a quitté la boucle à cause d'une contradiction)
    while (Variable::_endAssigned != Variable::_vars.end())
    {
        Variable * newAssign = NULL;
        Clause  * conflit = NULL;

        #if VERBOSE >= 4
        print_debug();
        std::cout << "Avant assign : ";
        print_vars();
        #endif
        // si pas de déduction : on doit faire un pari
        if(Variable::_endAssigned >= Variable::_endDeducted)
        {
            // choisit une variable libre, qu'on ajoute aux déductions
            Variable::chooseFromFree();
            // ajoute un choix libre pour le backtrack
            _stackBacktrack.push_back(Variable::_endAssigned);
            // assigne la variable
            newAssign = * (Variable::_endAssigned ++);
            #if VERBOSE >= 4
            print_debug();
            std::cout << "Assigne d'après un pari : ";
            newAssign->print_state();
            std::cout << std::endl;
            #endif
            conflit = newAssign->assignedFromDeducted();
        }
        // sinon : on doit faire attention si la variable vient d'une clause de taille 1
        else
        {
            // assigne la variable
            newAssign = * (Variable::_endAssigned ++);
            #if VERBOSE >= 4
            print_debug();
            std::cout << "Assigne d'après une déduction : ";
            newAssign->print_state();
            std::cout << std::endl;
            #endif
            conflit = newAssign->assignedFromDeducted();
            // si déduction depuis une clause à une seule variable, passe la variable en première assignation
            if (newAssign->getOriginClause() == NULL)
            {
                newAssign->moveToFirstAssign();
                // ne pas oublier d'aumenter de 1 la position de tous les paris
                for(unsigned i = 0; i < _stackBacktrack.size(); i++)
                    _stackBacktrack[i] ++;
            }
        }
        #if VERBOSE >= 4
        print_debug();
        std::cout << "Après assign : ";
        print_vars();
        #endif
 
        // On fait le backtrack si besoin
        if(conflit != NULL)
        {
            #if VERBOSE >= 4
            print_debug(); std::cout<<"Backtrack"<<std::endl;
            #endif
            // Si on n'a aucun choix libre, on renvoie faux (UNSAT)
            if (_stackBacktrack.empty())
                return false;

            // sinon : on apprend de nos erreurs
            std::pair<std::vector<Literal>,Literal> learned(resolve(conflit));
            
#if INTERACT
            if (nbLeftBeforePrompt == 0)
            {
                std::cout << "Conflit trouvé. La clause suivant a été apprise : ";
                for(unsigned k = 0; k < learned.first.size(); k++) {
                    if(learned.first[k].pos())
                        std::cout << '-';
                    std::cout << learned.first[k].var()->varNumber << ", ";
                }
                std::cout << std::endl << "Entrez l'une des options suivantes : g, r, c , s [n]" << std::endl;
                char readCar;
                bool goOn = true;
                while (goOn)
                {
                    goOn = false;
                    std::cin >> readCar;
                    
                    if (readCar == 'g')
                    {
                        createConflictGraph(conflit);
                        std::cout << "Graphe crée dans le fichier graph.dot" << std::endl;
                    }
                    else if (readCar == 'r')
                        std::cout << "Pas encore implémenté." << std::endl;
                    else if (readCar == 'c');
                    else if (readCar == 's')
                    {
                        int readInt=0;
                        if (std::cin.peek() == '\n')
                        {
                            std::string foo;
                            std::getline(std::cin, foo);
                        }
                        while (std::cin.peek() < '1' || std::cin.peek() > '9')
                        {
                            std::string foo;
                            std::getline(std::cin, foo);
                            std::cout<< "Veuillez entrer un nombre entier plus grand que 1." << std::endl;
                        }
                        std::cin >> readInt;
                        nbLeftBeforePrompt = readInt-1;
                    }
                    else if (readCar == 't')
                        nbLeftBeforePrompt = -1;
                    else
                    {
                        std::cout << "Veuillez entrer une option valide (g,r,c,s,t)." << std::endl;
                        goOn = true;
                    }
                }
            }
            else if (nbLeftBeforePrompt > 0)
                nbLeftBeforePrompt--;
#endif

            // On revient au dernier choix libre fait
            std::vector<Variable*>::iterator it, lastChoice = _stackBacktrack.back();
            _stackBacktrack.pop_back();
            do {
                Variable * var = * (-- Variable::_endAssigned);
                #if VERBOSE >= 5
                print_debug(); std::cout<<"Retour sur la valeur de la variable "<<var->varNumber<<std::endl;
                #endif
                // On libère la variable, des clauses où elle était surveillée
                var->deductedFromAssigned();
            } while (Variable::_endAssigned > lastChoice);
            Variable::_endDeducted = lastChoice;
            
            // on ajoute ce qu'on a appris comme déduction
            addClause(learned.first, learned.second);
            
            #if VERBOSE >= 4
            print_debug();
            std::cout << "Fin du backtrack : ";
            print_vars();
            #endif
        }
    }
    return true;
}



inline bool litCompVar(const Literal& lit1, const Literal& lit2)
{
    return lit1.var() < lit2.var();
}


struct compare
{
    inline bool operator () (const Literal& lit1, const Literal& lit2) const
    {
	    return lit1.var()->isOlder(lit2.var());
    }
};



std::pair<std::vector<Literal>,Literal> SatProblem::resolve(const Clause *conflictClause) const
{
    //std::cout << "debut resolve" << std::endl;
    #if VERBOSE >= 5
        print_debug();
        std::cout << "Resolve sur la clause " << conflictClause->clauseNumber << " : ";
        print_vars();
    #endif

    std::vector<Literal> mergedLits(conflictClause->getLiterals());
    
    sort(mergedLits.begin(), mergedLits.end(), litCompVar);
    
	const std::vector<Variable*>::iterator backtrackBack = _stackBacktrack.back();
	std::priority_queue<Literal, std::vector<Literal>, compare> toConsider;
	std::vector<bool> seen(this->_nbrVars+1, false);
	for (std::vector<Literal>::const_iterator it = mergedLits.begin(); it != mergedLits.end(); ++it)
	{
		if (it->var()->isFromCurBet(backtrackBack))
        {
			seen[it->var()->varNumber] = true;
			toConsider.push(*it);
        }
	}
    while (true)
    {
	#if false
        for (std::vector<Literal>::const_iterator it = mergedLits.begin();it != mergedLits.end(); ++it)
        {
            if (it->var()->isFromCurBet(backtrackBack))
            {
                #if VERBOSE >= 8
                print_debug();
                std::cout << "resolve : variable du pari courant trouvée : " << it->var()->varNumber << std::endl;
                #endif
                if (youngest.var()->isOlder(it->var()))
                    youngest = * it;
            }
        }
	#endif
        
       #if VERBOSE > 1
        if(toConsider.empty() || toConsider.top().var() == NULL) {
            std::cout << "c ATTENTION : ceci ne devrait pas arriver" << std::endl;
            break;
        }
        #endif
        
        if(toConsider.size() == 1)
            break;
        else if (toConsider.empty())
        {
            std::cout << "pas arriver :'(" << std::endl;
            break;
        }
        Literal youngest = toConsider.top();
		toConsider.pop();
		/*if (seen[youngest.var()->varNumber])
			continue;
		seen[youngest.var()->varNumber] = true;*/
        //std::cout << "lol" << (toConsider.top().var()->isOlder(youngest.var())) << std::endl;
        Clause *deductedFrom = youngest.var()->getOriginClause();
		
        std::vector<Literal> toMerge((deductedFrom == NULL) ? std::vector<Literal>(1,youngest) : deductedFrom->getLiterals());
		for (std::vector<Literal>::const_iterator toMergeIt = toMerge.begin(); toMergeIt != toMerge.end(); ++toMergeIt)
		{
			if (toMergeIt->var()->isFromCurBet(backtrackBack))
            {
                if (!seen[toMergeIt->var()->varNumber])
				{
					seen[toMergeIt->var()->varNumber] = true;
    				toConsider.push(*toMergeIt);
				}
            }
        }
        #if VERBOSE >= 7
        print_debug();
        std::cout << "resolve : merge la clause ";
        if (deductedFrom == NULL)
            std::cout << "de taille 1";
        else
            std::cout << deductedFrom->clauseNumber;
        std::cout << " qui a permis de déduire la variable " << youngest.var()->varNumber << std::endl;
        #endif
        sort(toMerge.begin(), toMerge.end(), litCompVar);
        
        std::vector<Literal> res(mergedLits.size()+toMerge.size());
        std::vector<Literal>::iterator resIt;
        resIt = std::set_union(mergedLits.begin(), mergedLits.end(), toMerge.begin(), toMerge.end(), res.begin(), litCompVar);
        res.resize(resIt-res.begin());
        res.erase(std::lower_bound(res.begin(), res.end(), youngest, litCompVar));
        std::swap(mergedLits, res);
    }

    #if VERBOSE >= 5
        print_debug();
        std::cout << "Nouvelle clause calculée : ";
        for(std::vector<Literal>::const_iterator it = mergedLits.begin(); it != mergedLits.end(); ++it)
            std::cout << it->var()->varNumber << '.' << it->pos() << ", ";
        std::cout << std::endl;
    #endif
//    std::cout << "fin resolve" << std::endl;
    return std::pair<std::vector<Literal>,Literal>(mergedLits, toConsider.top());
}
