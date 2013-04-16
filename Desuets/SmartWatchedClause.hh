#ifndef SMARTWATCHEDWATCHED_HH
#define SMARTWATCHEDWATCHED_HH

#include <vector>
#include "Literal.hh"




class SmartWatchedClause
{
public:
    SmartWatchedClause(const CONSTR_ARGS(list), Variable *firstTrue);

    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    ~SmartWatchedClause();

    const unsigned _number;
protected:
    /* Les watched literals sont les deux premiers de ce tableau. */
    std::vector<Literal> _lits;
    /* Indique si le deuxième watched du tableau est vraiment surveillé ou pas
       (optimisation « smart » semblable à SmartClause, mais pour les watched) */
    bool _watchSecond;
};





/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/



// TODO: affichage de debug pour ajout de clause en cours de route

// Quand on initialise une clause, on définit les watched litérals
inline SmartWatchedClause::SmartWatchedClause(const CONSTR_ARGS(list), Variable *firstTrue)
    : INIT_FOR_VERBOSE() _lits(list), _watchSecond(true)
{
    if (firstTrue)
    {
        _watchSecond = false;
        if (list[0].var() != firstTrue)
        {
            std::vector<Literal>::iterator it;
            for (it = _lits.begin(); it->var() != firstTrue; ++it);
            std::iter_swap(it, _lits.begin());
        }
        _lits[0].var()->linkToClause(_lits[0].pos(), (Clause*)this);
        //_lits[1].var()->linkToClause(_lits[1].pos(), (Clause*)this); Je crois qu'on doit pas faire ça
    }

    else
    {
        list[0].var()->linkToClause(list[0].pos(), (Clause*)this);
        list[1].var()->linkToClause(list[1].pos(), (Clause*)this);
        #if VERBOSE > 5
        std::cout << "Watched Lit (" << _number << ") : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                  << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
    }
}




inline bool SmartWatchedClause::setLitFalse(const Literal& lit)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse " << _number << " : " << lit.var()->varNumber << "." << lit.pos()
              << " (watched " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << ")"<< std::endl;
    #endif
    if (_lits[0].isTrue())
    {
        _watchSecond = false;
        return true;
    }
    // Échange _lits[0] et _lits[1] pour que lit soit le watched 0
    if (lit.var() == _lits[1].var())
        std::swap(_lits[0], _lits[1]);
    /* si l'un des litéraux est vrai, posFree pointe sur ce litéral.
       sinon il pointe sur un litéral FREE (ou sur end() s'il n'y en a pas). */
    std::vector<Literal>::iterator it, posFree = _lits.end();
    for (it = _lits.begin()+2; it != _lits.end(); ++it)
    {
        const varState v = it->var()->_varState;
        if (v == FREE)
            posFree = it;
        else if (it->pos() == (v == TRUE))
        {
            posFree = it;
            break;
        }
    }
    // posFree devient le nouveau litéral surveillé
    if (posFree != _lits.end())
    {
        posFree->var()->linkToClause(posFree->pos(), (Clause*)this);
        std::swap(_lits[0], *posFree);
        #if VERBOSE > 5
            std::cout << "new watched : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                      << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
    }
    return posFree == _lits.end();
}



inline bool SmartWatchedClause::setLitTrue(const Literal& l)
{
    if (l.var() == _lits[0].var())
    {
        return false;
    }
    else
    {
        if(_lits[0].isTrue())
        {
            _watchSecond = false;
            return true;
        }
        else
        {
            std::swap(_lits[0], _lits[1]);
            return false;
        }
    }
}



// Il est nécessaire de recommencer à surveiller watched[1] si _watchSecond
inline void SmartWatchedClause::freeLitTrue(const Literal& l)
{
    if (!_watchSecond)
    {
        _watchSecond = true;
        _lits[1].var()->linkToClause(_lits[1].pos(), (Clause*)this);
    }
}


inline void SmartWatchedClause::freeLitFalse(const Literal& l)
{
}



inline unsigned int SmartWatchedClause::freeSize() const
{
    // Utilise la conversion true=>1, false=>0
    return (_lits[0].var()->_varState == FREE)
         + (_lits[1].var()->_varState == FREE);
}


inline Literal SmartWatchedClause::getRemaining() const
{
    if (_lits[0].var()->_varState == FREE)
        return _lits[0];
    else
        return _lits[1];
}

inline bool SmartWatchedClause::isSatisfied() const
{
    return _lits[0].isTrue() || _lits[1].isTrue();
}



inline SmartWatchedClause::~SmartWatchedClause()
{
}

#endif // SMARTWATCHEDCLAUSE_HH defined
