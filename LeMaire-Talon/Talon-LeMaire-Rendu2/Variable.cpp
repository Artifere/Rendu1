#include "Variable.hh"
#include "Clause.hh"
#if VERBOSE > 0
#include <iostream>
#endif


/* Propage l'assignation d'une variable dans toutes les clauses dans lesquelles elle apparaît
   et arrête de surveiller une clause si setLitTrue/False renvoie true */
bool Variable::propagateVariable(std::stack<Literal>& deductions)
{
    bool isError = false;
    const bool isTrue = _varState == TRUE;
    const Literal lit = Literal(this, isTrue);

    std::vector<Clause*>& cTrue  = isTrue ? _litTrue : _litFalse;
    std::vector<Clause*>& cFalse = isTrue ? _litFalse : _litTrue;
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
                isError = true;
                break;
            }
            // Sinon, s'il n'y a pas déduction, ne rien faire. S'il y a une déduction on teste si elle est contradictoire
            else if(fs == 1)
            {
                const Literal deduct = target->getRemaining();
                #if VERBOSE > 5
                std::cout << "c Nouvelle déduction :  " << deduct.var()->varNumber << "." << deduct.pos() << std::endl;
                #endif
                // Si la déduction concerne une nouvelle variable, on l'ajoute
                if(deduct.var()->_varState == FREE)
                {
                    deductions.push(deduct);
                    deduct.var()->_varState = deduct.pos() ? TRUE:FALSE;
                }
                /* Sinon, si la déduction a déjà été faite, on ne fait rien.
                   Sinon, si on a déjà fait une déduction contraire, on a une contraduction. */
                else if(deduct.pos() != (deduct.var()->_varState == TRUE))
                {
                    isError = true;
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
void Variable::releaseVariable(void)
{
    const bool isTrue = _varState == TRUE;
    const Literal lit = Literal(this, isTrue);
    const std::vector<Clause*>& cTrue  = isTrue ? _litTrue : _litFalse;
    const std::vector<Clause*>& cFalse = isTrue ? _litFalse : _litTrue;
    std::vector<Clause*>::const_iterator it;

    for(it = cTrue.begin(); it != cTrue.end(); ++it)
        (*it)->freeLitTrue(lit);

    for(it = cFalse.begin(); it != cFalse.end(); ++it)
        (*it)->freeLitFalse(lit);
}

