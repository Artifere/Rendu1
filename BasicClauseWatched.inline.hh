/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClauseWatched.hh"
#ifndef BASICCLAUSEWATCHED_INLINE_HH
#define BASICCLAUSEWATCHED_INLINE_HH

#if VERBOSE > 0
#include <iostream>
#endif

static inline bool isLitTrue(const Literal& lit)
{
    const varState v = lit.var()->_varState;
    return (v == TRUE && lit.pos()) || (v == FALSE && !lit.pos());
}



inline BasicClauseWatched::BasicClauseWatched(CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE() _lits(list)
{
    list[0].var()->linkToClause(list[0].pos(), (StockedClause*)this);
    list[1].var()->linkToClause(list[1].pos(), (StockedClause*)this);
    #if VERBOSE > 5
    std::cout << "Watched Lit (" << _number << ") : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
    #endif
}



inline bool BasicClauseWatched::setLitFalse(const Literal& l)
{
    #if VERBOSE >= 10
    std::cout << "setLitFalse " << _number << " : " << l.var()->varNumber << "." << l.pos()
              << " (watched " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
              << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << ")"<< std::endl;
    #endif
    // échange _lts[0] et _lits[1] pour que l soit le watched 0
    if (l.var() == _lits[1].var())
        std::swap(_lits[0], _lits[1]);
    if(isLitTrue(_lits[1]))
        return false;
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
        posFree->var()->linkToClause(posFree->pos(), (StockedClause*)this);
        std::swap(_lits[0], *posFree);
        #if VERBOSE > 5
            std::cout << "new watched : " << _lits[0].var()->varNumber<<"."<<_lits[0].pos() << ", "
                      << _lits[1].var()->varNumber<<"."<<_lits[1].pos() << std::endl;
        #endif
        return true;
    } else
        return false;
}


inline bool BasicClauseWatched::setLitTrue(const Literal& l)
{
    return false;
}


inline void BasicClauseWatched::freeLitTrue(const Literal& l)
{
}


inline void BasicClauseWatched::freeLitFalse(const Literal& l)
{
}



inline size_t BasicClauseWatched::freeSize() const
{
    return (_lits[0].var()->_varState == FREE)
         + (_lits[1].var()->_varState == FREE);
    /*
    const bool b0 = _lits[0].var()->_varState == FREE,
               b1 = _lits[1].var()->_varState == FREE;
    if (b0 && b1)
        return 2;
    else if (b0 || b1)
        return 1;
    else
        return 0;
    */
}

inline Literal BasicClauseWatched::chooseFree() const
{
    if (_lits[0].var()->_varState == FREE)
        return _lits[0];
    else
        return _lits[1];
}

inline bool BasicClauseWatched::satisfied() const
{
    return isLitTrue(_lits[0]) || isLitTrue(_lits[1]);
    //const varState v = _lits[0].var()->_varState;
    //return (v == TRUE && _lits[0].pos()) || (v == FALSE && !_lits[0].pos());
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
