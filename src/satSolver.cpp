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

#include "Debug.hh"



const std::vector<std::pair<unsigned, bool> > SatProblem::getAssign(void) const
{
    typedef std::pair<unsigned,bool> returnType;
    std::vector<returnType> res(_nbrVars);

    for (unsigned k = 0; k < _nbrVars; k++)
        res[k] = returnType(Variable::_vars[k]->varNumber, Variable::_vars[k]->_varState);
    return res;
}


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
    if (problem.satisfiability())
        return 10;
    else
        return 20;
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
        return 10;
    }
    else
    {
        std::cout << "s UNSATISFIABLE\n";
        return 20;
    }
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
    : _nbrVars(nbrVar)//, debug(VERBOSE)
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
            DEBUG(3) << "Clause trivialement vraie lue (" << k+1 << "eme lue). Elle est ignorée." << std::endl;
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

    DEBUG(5) << "Etat des variables à la fin du parsage : " << *this << std::endl;
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
        DEBUG(1) << "Clause trivialement fausse lue (clause vide)."<<std::endl;
        #if VERBOSE
        std::cout<<"s UNSATISFIABLE"<<std::endl;
        #endif
        exit(0);
    }
    // clause de taille 1 : on ne la crée pas, mais on déduit la valeur de la variable
    else if(litsListSize == 1)
    {
        DEBUG(3) << "Clause à déduction immédiate lue : " << litsList[0] << std::endl;

        if (litsList[0].var()->isFree())
        {
            litsList[0].var()->deductedFromFree(litsList[0].pos(), NULL);            
        }
        if(!litsList[0].var()->isFree() && litsList[0].var()->_varState != litsList[0].pos())
        {
            #if VERBOSE
            std::cout<<"s UNSATISFIABLE"<<std::endl;
            #endif
            exit(0);
        }
    }
    // sinon : ajout réel de la clause, dans ce cas on on déduit un litéral de la clause, et l'autre
    else if(lit.var() == NULL)
    {
        newC = new Clause(litsList, number);
        _clauses.push_back(newC);
        DEBUG(7) << "Nouvelle clause " << number << " : " << litsList << std::endl;
    }
    else
    {
        newC = new Clause(litsList, number, lit.var());
        _clauses.push_back(newC);
        lit.var()->deductedFromFree(lit.pos(), newC);            
        DEBUG(7) << "Nouvelle clause " << number << " avec apprentissage de " << lit << " : " << litsList << std::endl;
    }
}





// Le coeur du solveur
bool SatProblem::satisfiability()
{
    // On continue l'exécution tant qu'on n'a pas assigné toutes les variables (ou que l'on a quitté la boucle à cause d'une contradiction)
    while (Variable::_endAssigned != Variable::_vars.end())
    {
        Variable * newAssign = NULL;
        Variable * conflit = NULL;

        DEBUG(5) << "Avant assign : " << *this << std::endl;

        // si pas de déduction : on doit faire un pari
        if(Variable::_endAssigned >= Variable::_endDeducted)
        {
            // choisit une variable libre, qu'on ajoute aux déductions
            // et on notifie le fait que ce choix était libre
            Variable::chooseFromFree();
            _stackBacktrack.push_back(Variable::_endAssigned);
            DEBUG(5) << "Assigne un pari : " << *(*Variable::_endAssigned) << std::endl;

            // assigne la variable
            newAssign = * (Variable::_endAssigned ++);
            conflit = newAssign->assignedFromDeducted();

            // cas particulier traitable immédiatement :
            // une clause qu'on avait pas vue permettait d'assigner newAssign à sa valeur contraire
            // on ne veut même pas que ça soit concidéré comme un backtrack : on fait son assignation contraire à la place
            if (conflit == newAssign)
            {
                DEBUG(6) << "M'enfin la clause " << newAssign->getOriginClause(! newAssign->_varState) << " permet de déduire " << Literal(newAssign, ! newAssign->_varState) << std::endl;
                // des-assigne la variable, et réassigne-là à sa valeur contraire
                // note : pas besoin de modifier deduct(True|False)FromClause de newAssign
                //        (il à déjà été placé à la bonne valeur dans deductFromAssigned)
                Variable::_endDeducted = Variable::_endAssigned; // on enlève les eventuelles déductions qu'aurait fait l'assignation
                newAssign->deductedFromAssigned();
                newAssign->_varState = ! newAssign->_varState;
                _stackBacktrack.pop_back(); // ne pas oublier d'enlever l'info sur le choix contraint
                conflit = newAssign->assignedFromDeducted();
            }
        }
        // sinon : on doit faire attention si la variable vient d'une clause de taille 1
        else
        {
            DEBUG(5) << "Assigne deduction : " << *(*Variable::_endAssigned) << std::endl;

            // assigne la variable
            newAssign = * (Variable::_endAssigned ++);
            conflit = newAssign->assignedFromDeducted();
        }

        DEBUG(4) << "Après propag : " << *this << std::endl;
        
        // On fait le backtrack si besoin
        if(conflit != NULL)
        {
            DEBUG(4) << "Backtrack sur " << *conflit << std::endl;

            
            // on revient au niveau du pari qui a causé l'erreur            
            while(!_stackBacktrack.empty() && !conflit->isFromCurBet(_stackBacktrack.back()))
            {
                DEBUG(6) << "\033[1;31mWTF DUDE ?\nBIG ERROR CATA-CATA PAS BIEN" << std::endl;
                _stackBacktrack.pop_back();
            }

            // Si on n'a aucun choix libre, on renvoie faux (UNSAT)
            if (_stackBacktrack.empty())
                return false;
            
            // sinon :
            // on apprend de nos erreurs
            std::pair<std::vector<Literal>,Literal> learned(resolve(conflit));

            #if INTERACT
            interact(learned, conflit);
            #endif

            // On revient au dernier choix libre fait (passe les variables de assignées à déduites)
            std::vector<Variable*>::iterator it, lastChoice = _stackBacktrack.back();
            _stackBacktrack.pop_back();
            do {
                Variable * var = * (-- Variable::_endAssigned);
                DEBUG(5) << "Retour sur la valeur de la variable " << *var << std::endl;
                // On libère la variable, des clauses où elle était surveillée
                var->deductedFromAssigned();
            } while (Variable::_endAssigned > lastChoice);
            
            DEBUG(4) << "sauvegrd |1| : " << *this << std::endl;

            // On annule toutes les déductions, sauf celles qui viennent d'une clause de taille 1
            // (attention : et on ne prend pas en compte la dernière déduction parce qu'elle correspond en réalité au choix libre qu'on vient d'annuler)
            bool isLearned = false; // si learned.second est parmis eux, on n'ajoutera pas la clause à la fin
            std::vector<Literal> oneDeducted;
            while (Variable::_endDeducted > lastChoice+1)
            {
                Variable* var = *(-- Variable::_endDeducted);
                if (var->getOriginClause(var->_varState) == NULL)
                {
                    DEBUG(7) << "Sauvegarde de " << *var << std::endl;
                    oneDeducted.push_back(Literal(var, var->_varState));
                    if (var == learned.second.var())
                        isLearned = true;
                }
            }
            Variable::_endDeducted = lastChoice;

            DEBUG(4) << "fin save |1| : " << *this << std::endl;
            
            // on ajoute ce qu'on a appris comme déduction
            if (! isLearned)
                addClause(learned.first, learned.second);

            // on reajoute toutes les déductions (qui viennent donc de clauses de taille 1)
            if (! oneDeducted.empty())
            {
                DEBUG(4) << "restaure |1| : " << *this << std::endl;
                do {
                    Literal& lit = oneDeducted.back();
                    oneDeducted.pop_back();
                    lit.var()->deductedFromFree(lit.pos(), NULL);
                    DEBUG(8) << "On récupère " << lit << std::endl;
                } while (! oneDeducted.empty());
            }
            
            DEBUG(4) << "Fin backtrck : " << *this << std::endl;
        }
    }
    return true;
}



static inline bool litCompVar(const Literal& lit1, const Literal& lit2)
{
    return lit1.var() < lit2.var();
}

static inline std::vector<Literal> getOriginClause(const Literal& lit)
{
    Clause* origin = lit.var()->getOriginClause(lit.pos());
    return origin ? origin->getLiterals() : std::vector<Literal>(1,lit);
}



std::pair<std::vector<Literal>,Literal> SatProblem::resolve(Variable *conflictVar) const
{
    Literal conflit(conflictVar, true);
    unsigned nbFromCurBet = 2;

    DEBUG(5) << "Resolve sur " << *conflictVar << " déduite à vrai par la clause " << conflictVar->getOriginClause(true) << " et à faux par la clause " << conflictVar->getOriginClause(false) << std::endl;
    
    std::vector<Literal> result(getOriginClause(conflit));
    std::sort(result.begin(), result.end(), litCompVar);

    // applique la résolution entre getOriginClause(result) et getOriginClause(conflit.invert)
    std::vector<Literal>::iterator resIt;
    do {
        DEBUG(8) << "fusionne la clause " << conflit.var()->getOriginClause(!conflit.pos()) << " qui à permis de déduire " << conflit.invert() << std::endl;

        std::vector<Literal> toMerge(getOriginClause(conflit.invert()));
        std::sort(toMerge.begin(), toMerge.end(), litCompVar);


        // deplace result dans otherToMerge, et met assez de place dans result pour la fusion
        std::vector<Literal> otherToMerge;
        std::swap(otherToMerge, result);

        result.resize(toMerge.size() + otherToMerge.size());
        
        // enlève l'occurence de conflit dans toMerge
        toMerge.erase(std::lower_bound(toMerge.begin(), toMerge.end(), conflit.invert(), litCompVar));

        // et l'occurence de conflit.invert dans otherToMerge
        otherToMerge.erase(std::lower_bound(otherToMerge.begin(), otherToMerge.end(), conflit, litCompVar));


        // fusionne les deux listes et enleve les doublons
        resIt = std::set_union(toMerge.begin(), toMerge.end(), otherToMerge.begin(), otherToMerge.end(), result.begin(), litCompVar);
 
        result.resize(resIt - result.begin());
        
        DEBUG(8) << "result : " << result << std::endl;

        // compte le nombre de litéraux du pari courant
        // et met la variable la plus jeune dans conflit
        // (on ne compte pas les littéraux qui viennent d'une clause de taille 1 comme vennant du pari courant)
        // note : case de taille 1 === pas le pari courant et originClause == NULL
        nbFromCurBet = 0;
        for(resIt = result.begin(); resIt != result.end(); resIt++)
        {
            Variable * var = resIt->var();
            if (!_stackBacktrack.empty() && var->isFromCurBet(_stackBacktrack.back()))// && !(var->getOriginClause(var->_varState) == NULL && var->isFromCurBet(_stackBacktrack.back()+1)))
            {
                DEBUG(8) << "resolve : variable du pari courant trouvée : " << * resIt << std::endl;

                if (nbFromCurBet == 0 || conflit.var()->isOlder(var))
                    conflit = * resIt;
                nbFromCurBet++;
            }
        }

    } while(nbFromCurBet >= 2);

    return std::pair<std::vector<Literal>,Literal>(result, conflit);
#if 0
    #if VERBOSE >= 5
        print_debug();
        std::cout << "Resolve sur la clause " << conflictClause->clauseNumber << " : ";
        print_vars();
    #endif

    std::vector<Literal> mergedLits(conflictClause->getLiterals());
    
    sort(mergedLits.begin(), mergedLits.end(), litCompVar);
    
    Literal youngest;
    while (true)
    {
        unsigned nbFromCurBet = 0;
        youngest = * mergedLits.begin();
        const std::vector<Literal>::const_iterator mergedEnd = mergedLits.end();
        for (std::vector<Literal>::const_iterator it = mergedLits.begin();it != mergedEnd; ++it)
        {
            if (it->var()->isFromCurBet(_stackBacktrack.back()))
            {
                #if VERBOSE >= 8
                print_debug();
                std::cout << "resolve : variable du pari courant trouvée : " << it->var()->varNumber << std::endl;
                #endif
                nbFromCurBet++;
                if (youngest.var()->isOlder(it->var()))
                    youngest = * it;
            }
        }
        
        if(nbFromCurBet <= 1) {
            break;
        }

        
        Clause *deductedFrom = youngest.var()->getOriginClause();
        std::vector<Literal> toMerge((deductedFrom == NULL) ? std::vector<Literal>(1,youngest) : deductedFrom->getLiterals());
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

    return std::pair<std::vector<Literal>,Literal>(mergedLits, youngest);
#endif
}



void SatProblem::interact(const std::pair<std::vector<Literal>,Literal>& learned, Variable* conflit)
{
    static int nbLeftBeforePrompt = 0;
    
    if (nbLeftBeforePrompt == 0)
    {
        std::cout << "Conflit trouvé. La clause suivant a été apprise : ";
        for(unsigned k = 0; k < learned.first.size(); k++) {
            if(learned.first[k].pos())
                std::cout << '-';
            std::cout << learned.first[k].var()->varNumber << ", ";
        }
        std::cout << std::endl << "Entrez l'une des options suivantes : g, r, c , s [n]" << std::endl;
        bool goOn = true;
        while (goOn)
        {
            char readCar;
            goOn = false;
            std::cin >> readCar;
            switch(readCar)
            {
            case 'g':
                createConflictGraph(conflit);
                std::cout << "Graphe crée dans le fichier graph.dot" << std::endl;
                break;

            case 'r':
                std::cout << "Pas encore implémenté." << std::endl;
                break;

            case 'c':
                break;

            case 's':
                int readInt;
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
                break;

            case 't':
                nbLeftBeforePrompt = -1;
                break;

            default:
                std::cout << "Veuillez entrer une option valide (g,r,c,s,t)." << std::endl;
                goOn = true;

            }
        }
    }
    else if (nbLeftBeforePrompt > 0)
    {
        nbLeftBeforePrompt--;
    }
}


