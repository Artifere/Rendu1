#ifndef WATCHEDCLAUSE_HH
#define WATCHEDCLAUSE_HH

#include <vector>
#include "Literal.hh"


class WatchedClause
{
public:
    WatchedClause(const CONSTR_ARGS(list));

    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    ~WatchedClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    /* Les watched literals sont les deux premiers de ce tableau.
       Après un setLitFalse, on doit avoir:
           si un des litéraux est vrai, un des deux watched est vrai aussi
           sinon, le nombre de FREE watched correspond au nombre de FREE de la clause
           (ou 2 si il y a plus de 2 FREE dans la clause au total)  */
    std::vector<Literal> _lits;
};




/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/


/* Quand on initialise une clause, copie les litéraux dans la clause,
   et on surveille uniquement les deux premiers */
inline WatchedClause::WatchedClause(const CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE() _lits(list)
{
    list[0].var()->linkToClause(list[0].pos(), (Clause*)this);
    list[1].var()->linkToClause(list[1].pos(), (Clause*)this);
    #if VERBOSE > 5
    std::cout << "Watched Lit (" << _number << ") : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
    #endif
}



/* Met un litéral à faux dans la clause. Ce litéral est forcement un des deux premiers.
   Cherche alors un autre litéral à surveiller :
     si un des litéraux est vrai, on le surveille
     et sinon, on surveille un litéral FREE.
     Si aucun des litéraux ne vérifie ces conditions, on ne change pas le watched */
inline bool WatchedClause::setLitFalse(const Literal& l)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse " << _number << " : " << l.var()->varNumber << "." << l.pos()
              << " (watched " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << ")"<< std::endl;
    #endif
    /* Ne tester qu'un seul des litéraux améliore les performances :o
       C'est un compromis entre tester les deux et ne rien tester
       (comme on appelle setLitFalse, ça signifie qu'au moins un des deux watched est faux) */ 
    if (_lits[0].isTrue()) //|| _lits[1].isTrue())
        return false;
    /* si l'un des litéraux est vrai, newWatched pointe sur ce litéral
       sinon il pointe sur un litéral FREE (ou sur end() s'il n'y en a pas) */
    const std::vector<Literal>::iterator end = _lits.end();
    std::vector<Literal>::iterator it, newWatched = end;
    for (it = _lits.begin()+2; it != end; ++it)
    {
        const varState v = it->var()->_varState;
        if (v == FREE)
            newWatched = it;
        else if(it->pos() == (v == TRUE))
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
        #if VERBOSE > 5
        std::cout << "new watched : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                  << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
    }
    return (newWatched != end);
}


/* on ne fait rien : le litéral passe à vrai, et reste surveillé */
inline bool WatchedClause::setLitTrue(const Literal& l)
{
    return false;
}



/* pas de backtrack pour les watched literals */
inline void WatchedClause::freeLitTrue(const Literal& l)
{
}

/* pas de backtrack pour les watched literals */
inline void WatchedClause::freeLitFalse(const Literal& l)
{
}



/* renvoie le nombre de FREE de la clause (ou 2 si le vrai résultat est >= 2)
   suppose que la clause n'est pas satisfaite et que freeSize est appellé après setLitFalse */
inline unsigned int WatchedClause::freeSize() const
{
    return (_lits[0].var()->_varState == FREE) // utilise la conversion true->1, false->0
         + (_lits[1].var()->_varState == FREE);
}

// Cette fonction est appelée quand il ne reste plus qu'une variable libre, et renvoie le litéral en question
inline Literal WatchedClause::getRemaining() const
{
    return _lits[_lits[1].var()->_varState == FREE]; // utilise la conversion true->1, false->0
}

// renvoie un résultat correct ssi appellé juste après setLitFalse
inline bool WatchedClause::isSatisfied() const
{
    return _lits[0].isTrue() || _lits[1].isTrue();
}



inline WatchedClause::~WatchedClause()
{
}

#endif // WATCHEDCLAUSE_HH defined
