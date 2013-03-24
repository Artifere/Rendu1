#include "Variable.hh"
#include "Clause.hh"
#if VERBOSE > 0
#include <iostream>
#endif

bool Variable::propagateVariable(std::stack<Literal>& deductions)
{
    bool isError = false;
    const bool isTrue = _varState == TRUE;
    const Literal lit = Literal(this, isTrue);

    std::vector<Clause*>& cTrue  = isTrue ? _litTrue : _litFalse;
    std::vector<Clause*>& cFalse = isTrue ? _litFalse : _litTrue;
    std::vector<Clause*>::iterator it;
    
    // propagation des litéraux true
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

    // on sépare en deux pour faire encore quelques tests de moins si il y a une erreur
    it = cFalse.begin();
    while (it != cFalse.end())
    {
        Clause* target = *it;
        if(target->setLitFalse(lit))
        {
            //if (it+1 != cFalse.end())
            *it = cFalse.back();
            cFalse.pop_back();
        }
        else
            ++it;
        
        if(!target->satisfied())
        {
            // si clause contradictoire : on renvoie une erreur
            const size_t fs = target->freeSize();
            if (fs == 0)
            {
                isError = true;
                break;
            }
            // sinon, si pas déduction, ne rien faire
            // et si déduction : on teste si elle n'est pas contradictoire
            else if(fs == 1)
            {
                const Literal deduct = target->chooseFree();
                #if VERBOSE > 5
                std::cout << "c Nouvelle déduction :  " << deduct.var()->varNumber << "." << deduct.pos() << std::endl;
                #endif
                // si la déduction concerne une nouvelle variable, on l'ajoute
                if(deduct.var()->_varState == FREE)
                {
                    deductions.push(deduct);
                    deduct.var()->_varState = deduct.pos() ? TRUE:FALSE;
                }
                // sinon, si déduction déjà faite, on ne fait rien
                // et si déduction contraire déjà faite, contradiction
                else if(deduct.pos() != (deduct.var()->_varState == TRUE))
                {
                    isError = true;
                    break;
                }
            }
        }
    }
    // on fini la propagation si il y a une erreur
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




void Variable::releaseVariable()
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




