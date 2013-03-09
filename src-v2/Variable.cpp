#include "Variable.hh"
#include "Clause.hh"
#include "BasicClause.hh"

bool Variable::propagateVariable(std::stack<Literal>& deductions)
{
    bool is_true = _varState == TRUE;
    const Literal lit = Literal(this, is_true);
    std::vector<StockedClause*>& cTrue  = is_true ? _litTrue : _litFalse;
    std::vector<StockedClause*>& cFalse = is_true ? _litFalse : _litTrue;

    bool is_error = false;

    std::vector<StockedClause*>::iterator it;
    for (it = cTrue.begin(); it != cTrue.end(); ++it)
        // on passe la clause à true : pas besoin de tester une déduction où une contradiction
        (*it)->setLitTrue(lit);

    // on sépare en deux pour faire encore quelques tests de moins si il y a une erreure
    // (comme je sais que tu t'inquiète de quelques tests ;)
    for (it = cFalse.begin(); (!is_error) && (it != cFalse.end()); ++it)
    {
        (*it)->setLitFalse(lit);
        // si clause contradictoire : on renvoie une erreur
        if (!(*it)->satisfied() && (*it)->freeSize() == 0)
            is_error = true;
        // sinon, si pas déduction, ne rien faire
        // et si déduction : on teste si elle n'est pas contradictoire
        else if( !(*it)->satisfied() && (*it)->freeSize() == 1)
        {
            Literal deduct = (*it)->chooseFree();
            // si la déduction concerne une nouvelle variable, on l'ajoute
            if(deduct.var()->_varState == FREE)
            {
                deductions.push(deduct);
                deduct.var()->_varState = deduct.pos() ? TRUE:FALSE;
            }
            // sinon, si déduction déjà faite, on ne fait rien
            // et si déduction contraire déjà faite, contradiction
            else if(deduct.pos() != (deduct.var()->_varState == TRUE))
                is_error = true;
        }
    }
    // on finit la propagation (si une erreur à eu lieu) mais sans essayer de trouver d'autres déductions
    for (; it != cFalse.end(); ++it)
        (*it)->setLitFalse(lit);
    return is_error;
}




void Variable::releaseVariable()
{
    bool is_true = _varState == TRUE;
    const Literal lit = Literal(this, is_true);
    std::vector<StockedClause*>& cTrue  = is_true ? _litTrue : _litFalse;
    std::vector<StockedClause*>& cFalse = is_true ? _litFalse : _litTrue;
    
    std::vector<StockedClause*>::iterator it;
    for(it = cTrue.begin(); it != cTrue.end(); ++it)
        (*it)->freeLitTrue(lit);

    for(it = cFalse.begin(); it != cFalse.end(); ++it)
        (*it)->freeLitFalse(lit);
}




