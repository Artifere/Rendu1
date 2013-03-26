#ifndef SMARTWATCHEDWATCHED_HH
#define SMARTWATCHEDWATCHED_HH

#include <vector>
#include "Literal.hh"




class SmartWatchedClause
{
public:
    SmartWatchedClause(const CONSTR_ARGS(list));

    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    ~SmartWatchedClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    /* Les watched literals sont les deux premiers de ce tableau. Si de plus un des litéraux surveillés est à vrai,
       On le met en première case du tableau. */
    std::vector<Literal> _lits;
    bool _watcheSecond;
};





/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/


// Quand on initialise une clause, on définit les watched litéraux
inline SmartWatchedClause::SmartWatchedClause(const CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE() _lits(list), _watcheSecond(true)
{
    list[0].var()->linkToClause(list[0].pos(), (Clause*)this);
    list[1].var()->linkToClause(list[1].pos(), (Clause*)this);
    #if VERBOSE > 5
    std::cout << "Watched Lit (" << _number << ") : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
    #endif
}



inline bool SmartWatchedClause::setLitFalse(const Literal& l)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse " << _number << " : " << l.var()->varNumber << "." << l.pos()
              << " (watched " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << ")"<< std::endl;
    #endif
    if (_lits[0].isTrue())
    {
        _watcheSecond = false;
        return true;
    }
    // échange _lts[0] et _lits[1] pour que l soit le watched 0
    if (l.var() == _lits[1].var())
        std::swap(_lits[0], _lits[1]);
    // si l'un des litéraux est vrai, posFree pointe sur ce litéral
    // sinon il pointe sur un litéral FREE (ou sur end() s'il n'y en a pas)
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
    // (on a donc encore :  si l'un des litéraux de la clause est vrai, alors _lits[0] est vrai)
    if (posFree != _lits.end())
    {
        //_lits[0].var()->unlinkToClause(_lits[0].pos(), (StockedClause*)this);
        posFree->var()->linkToClause(posFree->pos(), (Clause*)this);
        std::swap(_lits[0], *posFree);
        #if VERBOSE > 5
            std::cout << "new watched : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                      << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
        return true;
    } else
        return false;
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
            _watcheSecond = false;
            return true;
        }
        else
        {
            std::swap(_lits[0], _lits[1]);
            return false;
        }
    }
}



inline void SmartWatchedClause::freeLitTrue(const Literal& l)
{
    if (!_watcheSecond)
    {
        _watcheSecond = true;
        _lits[1].var()->linkToClause(_lits[1].pos(), (Clause*)this);
    }
}

inline void SmartWatchedClause::freeLitFalse(const Literal& l)
{
}



inline unsigned int SmartWatchedClause::freeSize() const
{
    // utilise la conversion tru=>1, false=>0
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
    //const varState v = _lits[0].var()->_varState;
    //return (v == TRUE && _lits[0].pos()) || (v == FALSE && !_lits[0].pos());
}



inline SmartWatchedClause::~SmartWatchedClause()
{
}

#endif //SMARTWATCHEDCLAUSE_HH
