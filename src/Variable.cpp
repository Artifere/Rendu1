#include "Variable.hh"
#include "Clause.hh"
#if VERBOSE > 0
#include <iostream>
#endif

std::vector<Variable*> Variable::_vars;
std::vector<Variable*>::iterator Variable::_endAssigned = _vars.begin();
std::vector<Variable*>::iterator Variable::_endDeducted = _vars.begin();



void Variable::chooseFromFree_BASIC(void)
{
    ++_endDeducted;
}



void Variable::chooseFromFree_DLIS(void)
{
    std::vector<Variable*>::iterator it;
    it = std::max_element(_endDeducted, _vars.end(), DLISvarCompr);
    std::swap((*_endDeducted)->_posInTable, (*it)->_posInTable);
    std::iter_swap(_endDeducted, it);
    ++_endDeducted;
}


void Variable::chooseFromFree_MOMS(void)
{
    std::vector<Variable*>::iterator freeVarIt = _endDeducted, bestVarIt;
    unsigned minSize = _vars.size()+1, maxNbr;
    bool bestPol;

    while (freeVarIt != _vars.end())
    {
        unsigned curMinSize = _vars.size(), curNbrMinSize = 0;

        for (std::vector<Clause*>::const_iterator trueIt = (*freeVarIt)->_litTrue.begin(); trueIt != (*freeVarIt)->_litTrue.end(); ++trueIt)
        {
            if (!(*trueIt)->isSatisfied())
            {
                if ((*trueIt)->freeSize() < curMinSize)
                {
                    curMinSize = (*trueIt)->freeSize();
                    curNbrMinSize = 1;
                }
                else if ((*trueIt)->freeSize() == curMinSize)
                {
                    curNbrMinSize++;
                }
            }
        }

        if (curMinSize < minSize || (curMinSize == minSize && curNbrMinSize < maxNbr))
        {
            bestVarIt = freeVarIt;
            bestPol = true;
            minSize = curMinSize;
            maxNbr = curNbrMinSize;
        }


        for (std::vector<Clause*>::const_iterator falseIt = (*freeVarIt)->_litFalse.begin(); falseIt != (*freeVarIt)->_litFalse.end(); ++falseIt)
        {
            if (!(*falseIt)->isSatisfied())
            {
                if ((*falseIt)->freeSize() < curMinSize)
                {
                    curMinSize = (*falseIt)->freeSize();
                    curNbrMinSize = 1;
                }
                else if ((*falseIt)->freeSize() == curMinSize)
                {
                    curNbrMinSize++;
                }
            }
        }

        if (curMinSize < minSize || (curMinSize == minSize && curNbrMinSize < maxNbr))
        {
            bestVarIt = freeVarIt;
            bestPol = false;
            minSize = curMinSize;
            maxNbr = curNbrMinSize;
        }



        ++freeVarIt;
    }

    //J'ai copié collé un code du dessus pour là, ça marche tu penses ?^^
    std::swap((*_endDeducted)->_posInTable, (*bestVarIt)->_posInTable);
    std::iter_swap(_endDeducted, bestVarIt);
    (*bestVarIt)->_varState = bestPol;
    ++_endDeducted;
}


void Variable::chooseFromFree_RAND(void)
{
    static bool isInit = false;
    if (!isInit)
    {
        isInit = true;
        srandom(time(NULL));
    }
    unsigned int ret = random() % (_vars.end() - _endDeducted);
    std::vector<Variable*>::iterator it;
    it = _endDeducted + ret;
    std::swap((*_endDeducted)->_posInTable, (*it)->_posInTable);
    std::iter_swap(_endDeducted, it);
    ++_endDeducted;
}









/* Propage l'assignation d'une variable dans toutes les clauses dans lesquelles elle apparaît
   et arrête de surveiller une clause si setLitTrue/False renvoie true */
Clause* Variable::assignedFromDeducted(void)
{
    Clause* isError = NULL;
    const Literal lit = Literal(this, _varState);
    
    #if VERBOSE >= 5
    std::cout << "Propagation du litéral :";
    this->print_state();
    std::cout << std::endl;
    #endif

    std::vector<Clause*>& cTrue  = _varState ? _litTrue : _litFalse;
    std::vector<Clause*>& cFalse = _varState ? _litFalse : _litTrue;
    std::vector<Clause*>::iterator it;

    // On propage les litéraux qui deviennent satisfaits
    it = cTrue.begin();
    while (it != cTrue.end())
    {
        if((*it)->setLitTrue(lit))
        {
            *it = cTrue.back();
            cTrue.pop_back();
        }
        else
            ++it;
    }

    // On sépare en deux pour faire quelques tests de moins s'il y a une erreur
    it = cFalse.begin();
    while (it != cFalse.end())
    {
        Clause* target = *it;
        if(target->setLitFalse(lit))
        {
            *it = cFalse.back();
            cFalse.pop_back();
        }
        else
            ++it;

        if(!target->isSatisfied())
        {
            // Si une clause est contradictoire : on renvoie une erreur
            const unsigned int fs = target->freeSize();
            if (fs == 0)
            {
                #if VERBOSE > 4
                std::cout << "c Contradiction (clause " <<target->clauseNumber<< ")" << std::endl;
                #endif
                //std::cout << "bobo" << std::endl;
                isError = target;
                break;
            }
            // Sinon, s'il n'y a pas déduction, ne rien faire. S'il y a une déduction on teste si elle est contradictoire
            else if(fs == 1)
            {
                const Literal deduct = target->getRemaining();
                #if VERBOSE > 4
                std::cout << "c Déduction trouvée (clause " <<target->clauseNumber<< ") :  " << deduct.var()->varNumber << "." << deduct.pos() << std::endl;
                #endif
                // Si la déduction concerne une nouvelle variable, on l'ajoute
                if(deduct.var()->isFree())
                    deduct.var()->deductedFromFree(deduct.pos(), target);
                /* Sinon, si la déduction a déjà été faite, on ne fait rien.
                   Sinon, si on a déjà fait une déduction contraire, on a une contraduction. */
                else if(deduct.pos() != deduct.var()->_varState)
                {
                    isError = target;
                    break;
                }
            }
        }
    }

    // On finit la propagation (n'arrive que s'il y a eu une erreur)
    while (it != cFalse.end())
    {
        if((*it)->setLitFalse(lit))
        {
            *it = cFalse.back();
            cFalse.pop_back();
        }
        else
            ++it;
    }
    return isError;
}




/* Annule les changements faits par propagateVariable dans les clauses contenant la variable.
   Cette fonction doit être appelée lors du backtrack. */
void Variable::deductedFromAssigned(void)
{
    const Literal lit = Literal(this, _varState);
    const std::vector<Clause*>& cTrue  = _varState ? _litTrue : _litFalse;
    const std::vector<Clause*>& cFalse = _varState ? _litFalse : _litTrue;
    std::vector<Clause*>::const_iterator it;

    for(it = cTrue.begin(); it != cTrue.end(); ++it)
        (*it)->freeLitTrue(lit);

    for(it = cFalse.begin(); it != cFalse.end(); ++it)
        (*it)->freeLitFalse(lit);
}




#if VERBOSE > 0
#include <iostream>
#include <iomanip>
void Variable::print_state(void) const
{
    if (isFree())
        std::cout << "?";
    else if (_varState)
        std::cout << "+";
    else
        std::cout << "-";
    std::cout << std::setw(2) << std::setfill('_') << varNumber << std::setw(1);
    //std::cout << '|' << (_posInTable - _vars.begin());
    #if VERBOSE > 10
    std::cout << '(';
    for(unsigned j = 0; j < _litTrue.size(); j++)
      std::cout << _litTrue[j]->clauseNumber << ".";
    std::cout << ", ";
    for(unsigned j = 0; j < _litFalse.size(); j++)
      std::cout << _litFalse[j]->clauseNumber << ".";
    std::cout << ')';
    #endif
}
#endif



