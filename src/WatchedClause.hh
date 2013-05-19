#ifndef WATCHEDCLAUSE_HH
#define WATCHEDCLAUSE_HH

#include <vector>
#include "Literal.hh"


class WatchedClause
{
public:
    // constructeur initial : doit marcher lorsque aucun litéral n'est assignée (certaines variables peuvent être déduites, mais aucune assignée)
    WatchedClause(const std::vector<Literal>& list, const unsigned number);
    // constructeur d'apprentissage : les litéraux sont soit libres, soit assignés à faux (pas de déduits, pas de litéraux vrais)
    WatchedClause(const std::vector<Literal>& list, const unsigned number, Variable * lastAssigned);

    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    const std::vector<Literal>& getLiterals(void) const;

    ~WatchedClause();

    const unsigned clauseNumber;
protected:
    /* Les watched literals sont les deux premiers de ce tableau.
       Après un setLitFalse, on doit avoir :
           si un des litéraux est vrai, un des deux watched est vrai aussi
           sinon, le nombre de FREE watched correspond au nombre de variables dans l'état FREE de la clause
           (ou 2 si il y a plus de 2 variables dans l'état FREE dans la clause au total)  */
    std::vector<Literal> _lits;

    friend std::ostream& operator<< (std::ostream& out, const WatchedClause*);
};




/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/



// TODO: affichage de debug pour ajout de clause en cours de route

// Quand on initialise une clause, on définit les watched litérals
inline WatchedClause::WatchedClause(const std::vector<Literal>& list, const unsigned int number)
    : clauseNumber(number), _lits(list)
{
    _lits[0].var()->linkToClause(_lits[0].pos(), (Clause*)this);
    _lits[1].var()->linkToClause(_lits[1].pos(), (Clause*)this);
    #if VERBOSE > 7
    std::cout << "Watched Lit (" << clauseNumber << ") : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
    #endif
}





inline WatchedClause::WatchedClause(const std::vector<Literal>& list, const unsigned int number, Variable * lastAssigned)
    : clauseNumber(number), _lits(list)
{
    std::vector<Literal>::iterator it;
    // place le plus jeune literal en _lits[0]
    for (it = _lits.begin()+1; it != _lits.end(); ++it)
    {
        if (_lits[0].var()->isOlder(it->var()))
            std::iter_swap(it, _lits.begin());
    }
    // place le second plus jeune literal en _lits[1]
    for (it = _lits.begin()+2; it != _lits.end(); ++it)
    {
        if (_lits[1].var()->isOlder(it->var()))
            std::iter_swap(it, _lits.begin()+1);
    }
    // surveille les deux premiers litéraux
    _lits[0].var()->linkToClause(_lits[0].pos(), (Clause*)this);
    _lits[1].var()->linkToClause(_lits[1].pos(), (Clause*)this);
    DEBUG(8) << "Watched Lit (" << clauseNumber << ") : " << _lits[0] << ", " << _lits[1] << std::endl;
}





/* Met un litéral à faux dans la clause. Ce litéral est forcement un des deux premiers.
   On cherche alors un autre litéral à surveiller :
     si un des litéraux est vrai, on le surveille
     sinon, on surveille un litéral FREE.
     Si aucun des litéraux ne vérifie ces conditions, on ne change pas le watched literal*/
inline bool WatchedClause::setLitFalse(const Literal& l)
{
    DEBUG(10) << "set  false(" << this << "):" << l << std::endl;
    /* Ne tester qu'un seul des litéraux améliore les performances :o
       C'est un compromis entre tester les deux et ne rien tester
       (comme on appelle setLitFalse, ça signifie qu'au moins un des deux watched est faux) */ 
    if (isSatisfied())
        return false;
    /* Si l'un des litéraux est vrai, newWatched pointe sur ce litéral
       sinon il pointe une litéral FREE (le plus jeune?) ou sur end() s'il n'y en a pas */
    const std::vector<Literal>::iterator end = _lits.end();
    std::vector<Literal>::iterator it, newWatched = end;
    for (it = _lits.begin()+2; it != end; ++it)
    {
        if (it->var()->isFree())
        {
            // à décommenter pour prendre la variable la plus jeune. vraiment utile ?
            //if(newWatched == end || newWatched->var()->isOlder(it->var()))
                newWatched = it;
        }
        else if(it->pos() == it->var()->_varState)
        {
            newWatched = it;
            break;
        }
    }
    if (newWatched != end)
    {
        // *newWatched devient le nouveau litéral surveillé
        newWatched->var()->linkToClause(newWatched->pos(), (Clause*)this);
        _lits[l.var() == _lits[1].var()] = *newWatched; // utilise la conversion true->1, false->0
        *newWatched = l.invert();
        DEBUG(8) << "new watched : " << _lits[0] << ", " << _lits[1] << std::endl;
    }
    return (newWatched != end);
}


// On ne fait rien : le litéral passe à vrai, et reste surveillé
inline bool WatchedClause::setLitTrue(const Literal& l)
{
    DEBUG(10) << "set  true (" << clauseNumber << "):" << l << std::endl;
    return false;
}



// Pas de backtrack pour les watched literals
inline void WatchedClause::freeLitTrue(const Literal& l)
{
    DEBUG(10) << "free true (" << clauseNumber << "):" << l << std::endl;
}

// Pas de backtrack pour les watched literals
inline void WatchedClause::freeLitFalse(const Literal& l)
{
    DEBUG(10) << "free false(" << clauseNumber << "):" << l << std::endl;
}



/* On renvoie le nombre de FREE de la clause (ou 2 si le vrai résultat est >= 2).
   On suppose que la clause n'est pas satisfaite et que freeSize est appelé après setLitFalse */
inline unsigned int WatchedClause::freeSize() const
{
    return (_lits[0].var()->isFree()) // utilise la conversion true->1, false->0
         + (_lits[1].var()->isFree());
}

// Cette fonction est appelée quand il ne reste plus qu'une variable libre, et renvoie le litéral en question
inline Literal WatchedClause::getRemaining() const
{
    return _lits[_lits[1].var()->isFree()]; // utilise la conversion true->1, false->0
}

// Renvoie un résultat correct ssi appelé juste après setLitFalse
inline bool WatchedClause::isSatisfied() const
{
    return _lits[0].isTrue() || _lits[1].isTrue();
}



inline WatchedClause::~WatchedClause()
{
}




inline const std::vector<Literal>& WatchedClause::getLiterals(void) const
{
    return _lits;
}


#endif // WATCHEDCLAUSE_HH defined
