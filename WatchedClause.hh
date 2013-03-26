#include "Clause.hh"
#ifndef WATCHEDCLAUSE_HH
#define WATCHEDCLAUSE_HH

#include <vector>

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
    // les deux premers éléments du tableau sont surveillés
    // si on trouve un litéral vrai, on le surveille dans _literals[0]
    std::vector<Literal> _lits;

};




/***
 * Implementation des methodes de la classe
 * (toutes inlines)
***/


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



inline bool WatchedClause::setLitFalse(const Literal& l)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse " << _number << " : " << l.var()->varNumber << "." << l.pos()
              << " (watched " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << ")"<< std::endl;
    #endif
    if (_lits[0].isTrue())//|| _lits[1].isTrue()) // possible optimisation. ne semble pas gagner grand chose
        return false;
    // si l'un des litéraux est vrai, newWatched pointe sur ce litéral
    // sinon il pointe sur un litéral FREE (ou sur end() s'il n'y en a pas)
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
         // ne fait que deux affectations (au lieu de trois pour un swap)
        _lits[l.var() == _lits[1].var()] = *newWatched; // utilise la conversion true->1, false->0
        *newWatched = l.invert();
        #if VERBOSE > 5
            std::cout << "new watched : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                      << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
    }
    return (newWatched != end);
}

inline bool WatchedClause::setLitTrue(const Literal& l)
{
    //if (l.var() == _lits[0].var())//!isLitTrue(_lits[0]))
    //    std::swap(_lits[0], _lits[1]);
    return false;
}



inline void WatchedClause::freeLitTrue(const Literal& l)
{
}

inline void WatchedClause::freeLitFalse(const Literal& l)
{
}



inline unsigned int WatchedClause::freeSize() const
{
    return (_lits[0].var()->_varState == FREE) // utilise la conversion true->1, false->0
         + (_lits[1].var()->_varState == FREE);
}

inline Literal WatchedClause::getRemaining() const
{
    return _lits[_lits[1].var()->_varState == FREE]; // utilise la conversion true->1, false->0
}

inline bool WatchedClause::isSatisfied() const
{
    return _lits[0].isTrue() || _lits[1].isTrue();
}



inline WatchedClause::~WatchedClause()
{
}

#endif //WATCHEDCLAUSE_HH
