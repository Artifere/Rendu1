/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "OneWatchedClause.hh"
#ifndef ONEWATCHEDCLAUSE_INLINE_HH
#define ONEWATCHEDCLAUSE_INLINE_HH

#if VERBOSE > 0
#include <iostream>
#endif

inline OneWatchedSmartClause::OneWatchedSmartClause(CONSTR_ARGS(list))
    :INIT_FOR_VERBOSE()  _literals(list), _numOfFree(list.size())
{
    _literals[0].var()->linkToClause(_literals[0].pos(), (StockedClause*)this);
    #if VERBOSE > 5
    std::cout << "Watched Lit (" << _number << ") : " << _watched.var()->varNumber<<"."<<_watched.pos()<<std::endl;
    #endif
}


inline void OneWatchedSmartClause::setLitFalse(const Literal& l)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse "<<_number<<" : "<<l.var()->varNumber<<"."<<l.pos()<<" (watched "<<_watched.var()->varNumber<<"."<<_watched.pos()<<")"<<std::endl;
    #endif
    _numOfFree = 0;
    std::vector<Literal>::const_iterator it;
        for (it = _literals.begin(); it != _literals.end(); it++)
        {
            if(it->var()->_varState == FREE)
                break;
        }
        if(it != _literals.end())
        {
            _watched.var()->unlinkToClause(_watched.pos(), (StockedClause*)this);
            _watched = it->invert();
            _watched.var()->linkToClause(_watched.pos(), (StockedClause*)this);
            #if VERBOSE >= 10
            std::cout << "Change Watched " << _number << " : " << _watched.var()->varNumber<< "." << _watched.pos() <<std::endl;
            #endif
        }
    }
    else
    {
            _watched.var()->unlinkToClause(_watched.pos(), (StockedClause*)this);
            _watched = l;
            _watched.var()->linkToClause(_watched.pos(), (StockedClause*)this);
            #if VERBOSE >= 10
            std::cout << "Change Watched (clause true) " << _number << " : " << _watched.var()->varNumber<< "." << _watched.pos() <<std::endl;
            #endif
    }
    // changement de watched litteral
    }
}
inline void OneWatchedClause::setLitTrue(const Literal& l)
{
    _numOfFree = 0;
    //std::cout << "Toto " << _number << " : " << _watched.var()->varNumber << "." << _watched.pos() << " : " << l.var()->varNumber << "." << l.pos() << std::endl;
}


inline void OneWatchedClause::freeLitTrue(const Literal& l)
{
}
inline void OneWatchedClause::freeLitFalse(const Literal& l)
{
}


inline size_t OneWatchedClause::freeSize (void) const
{
    size_t res = 0;
    std::vector<Literal>::const_iterator it;
    for (it = _literals.begin(); it != _literals.end(); it++)
        if(it->var()->_varState == FREE) res++;
    return res;
}

inline Literal OneWatchedClause::chooseFree(void) const
{
    #if VERBOSE >= 10
    std::cout << "c Deduct from clause " << _number << " : " << _watched.var()->varNumber << "." << _watched.pos() << std::endl;
    #endif
    return _watched;
}
inline bool OneWatchedClause::satisfied(void) const
{
    return (_watched.var()->_varState == TRUE && _watched.pos()) || (_watched.var()->_varState == FALSE && !_watched.pos());
}

inline OneWatchedClause::~OneWatchedClause() { }

#endif//ONEWATCHEDCLAUSE_INLINE_HH