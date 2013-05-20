#include "Variable.hh"
#include "Clause.hh"

#include "Debug.hh"


/* variables "globales" (doivent être déclarée dans un fichier .cpp) */
std::vector<Variable*> Variable::_vars;
std::vector<Variable*>::iterator Variable::_endAssigned = _vars.begin();
std::vector<Variable*>::iterator Variable::_endDeducted = _vars.begin();



/* Fonction de choix de variable basique : on prend la première du tableau */
void Variable::chooseFromFree_BASIC(void)
{
    ++_endDeducted;
}



/* Choix de variable selon DLIS */
void Variable::chooseFromFree_DLIS(void)
{
    std::vector<Variable*>::iterator it;
    it = std::max_element(_endDeducted, _vars.end(), DLISvarCompr);
    (*it)->_varState = (*it)->sizeLitTrue() > (*it)->sizeLitFalse();
    std::swap((*_endDeducted)->_posInTable, (*it)->_posInTable);
    std::iter_swap(_endDeducted, it);
    ++_endDeducted;
}




/* Choix de variable avec MOMS. Attention : implémentation naïve donc lente, et n'est pas réellement MOMS avec les watched */
void Variable::chooseFromFree_MOMS(void)
{
    std::vector<Variable*>::iterator  bestVarIt;
    const std::vector<Variable*>::iterator endVar = _vars.end();
    unsigned minSize = _vars.size()+1, maxNbr = (unsigned)(-1);
    bool bestPol = false;

    for (std::vector<Variable*>::iterator freeVarIt= _endDeducted; freeVarIt != endVar; ++freeVarIt)
    {
        unsigned curMinSize = _vars.size(), curNbrMinSize = 0;

        const std::vector<Clause*>::const_iterator endTrue = (*freeVarIt)->_litTrue.end();
        for (std::vector<Clause*>::const_iterator trueIt = (*freeVarIt)->_litTrue.begin(); trueIt != endTrue; ++trueIt)
        {
            // attention, dans le cas des watched literals, isSatisfied et freeSize renvoie un résultat invalide
            // ils ne renvoie un réultat valide que si on vient de mettre un litéral de la clause à false, et qu'on a fait aucune assignation ou deduction de litéral depuis.
            // mais même si le résultat est faux, ça ne devrait pas partir an boucle infinie à cause de ça (c'est juste qu'on a pas MOMS, mais un truc un peu bizare)
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
        curNbrMinSize = 0;

        // tu ne remet pas à 0 (enfin à _vars.size()) curMinSize ?
        // si non, pourquoi le bloc précédent ? il ne suffit pas de le mettre une fois pour tout à la fin de la fonction ?

        //==> je ne comprends pas tout. Mais je ne remets pas curMinSize à _var.size, parce que si on trouve pas plus petit que ce qu'on avait, ça sert à rien
        //==> en revance, faut que je mette à 0 curNbrMinSize, sinon si on a une taille identique après, il va croise que c'est le même truc :s
        //==> ah, je crois que je commence à comprendre, je réfléchis...
        //======> le problème je crois que c'est maxNbr, des fois il faut le mettre à jour. Si tu veux y réfléchir plus, libre à toi, pour l'instant je laisse comme ça

        const std::vector<Clause*>::const_iterator endFalse = (*freeVarIt)->_litFalse.end();
        for (std::vector<Clause*>::const_iterator falseIt = (*freeVarIt)->_litFalse.begin(); falseIt != endFalse; ++falseIt)
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
    }

    (*bestVarIt)->_varState = bestPol;
    std::swap((*_endDeducted)->_posInTable, (*bestVarIt)->_posInTable);
    std::iter_swap(_endDeducted, bestVarIt);
    ++_endDeducted;
}



/* Choix aléatoire ==> vraiment pas efficace comme heuristique... */
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
Variable* Variable::assignedFromDeducted(void)
{
    Variable* error = NULL;
    const Literal lit = Literal(this, _varState);
    
    DEBUG(6) << "Propagation du litéral : " << *this << std::endl;

    std::vector<Clause*>& cTrue  = _varState ? _litTrue : _litFalse;
    std::vector<Clause*>& cFalse = _varState ? _litFalse : _litTrue;
    std::vector<Clause*>::iterator it;

    /* On propage les litéraux qui deviennent satisfaits */
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

    /* On sépare en deux pour faire quelques tests de moins s'il y a une erreur */
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
            const unsigned int fs = target->freeSize();
            /* Si une clause est contradictoire : on renvoie une erreur */
            if (fs == 0)
            {
                DEBUG(5) << "Contradiction (clause " << target << ", variable " << varNumber << ")" << std::endl;
                /* L'ereur vient de la variable qu'on est en train d'assigner */
                error = this;
                if (!_varState)
                    _deductedTrueFromClause = target;
                else
                    _deductedFalseFromClause = target;
                break;
            }

            /* Sinon, s'il n'y a pas déduction, ne rien faire. S'il y a une déduction on teste si elle est contradictoire */
            else if(fs == 1)
            {
                const Literal deduct = target->getRemaining();

                /* Si la déduction concerne une nouvelle variable, on l'ajoute */
                if(deduct.var()->isOlderIter(_endDeducted))
                {
                    deduct.var()->deductedFromFree(deduct.pos(), target);
                    DEBUG(5) << "Nouvelle déduction trouvée (clause " << target << ") :  " << deduct << std::endl;
                }
                /* Sinon, si on a déjà fait une déduction contraire, on a une contraduction. */
                else if(deduct.pos() != deduct.var()->_varState)
                {
                    DEBUG(5) << "Déduction contradictoire trouvée (clause " << target << ") :  " << deduct << std::endl;
                    error = deduct.var();
                    if(deduct.pos())
                        deduct.var()->_deductedTrueFromClause = target;
                    else
                        deduct.var()->_deductedFalseFromClause = target;
                    break;
                }
                /* Sinon, si on a déjà fait la même déduction, on ne fait rien. */
                else
                {
                    DEBUG(6) << "Déduction (re-)trouvée (clause " << target << ") :  " << deduct << std::endl;
                }
            }
        }
    }

    /*  On finit la propagation (n'arrive que s'il y a eu une erreur) */
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
    return error;
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




