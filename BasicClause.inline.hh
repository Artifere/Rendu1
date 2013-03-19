/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClause.hh"
#ifndef BASICCLAUSE_INLINE_HH
#define BASICCLAUSE_INLINE_HH

#include <cstdlib> // pour exit()



inline BasicClause::BasicClause(const CONSTR_ARGS(list))
    :  INIT_FOR_VERBOSE()  _satisfied(false), _free(list.begin(),list.end())
{
    std::vector<Literal>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it)
        it->var()->linkToClause(it->pos(), (Clause*)this);
}



inline bool BasicClause::setLitFalse(const Literal& l)
{
    if(! _satisfied)
    {
        Literal invL = l.invert();
        _free.erase(invL);
        _assigned.push(invL);
    }
    return false;
}

inline bool BasicClause::setLitTrue(const Literal& l)
{
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
    return false;
}



inline void BasicClause::freeLitTrue(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}

inline void BasicClause::freeLitFalse(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}



inline size_t BasicClause::freeSize(void) const
{
    return _free.size();
}

inline Literal BasicClause::chooseFree(void) const
{
    return *_free.begin();
}

inline bool BasicClause::satisfied(void) const
{
    return _satisfied;
}



inline BasicClause::~BasicClause()
{
}

#endif//BASICCLAUSE_INLINE_HH
