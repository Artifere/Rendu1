/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClause.hh"
#ifndef BASICCLAUSE_INLINE_HH
#define BASICCLAUSE_INLINE_HH

inline BasicClauseWatched::BasicClauseWatched(const std::vector<Literal>& list)
{
    _satisfied = false;
    for (size_t i = 0; i < list.size(); ++i)
        _free.insert(list[i]);
}


inline void BasicClauseWatched::setLitFalse(const Literal& l)
{
    if(! _satisfied)
    {
        Literal invL = l;
        invL.invert();

        _free.erase(invL);
        _assigned.push(invL);
    }
}
inline void BasicClauseWatched::setLitTrue(const Literal& l)
{
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
}


inline void BasicClauseWatched::freeLitTrue(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}
inline void BasicClauseWatched::freeLitFalse(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}


inline size_t BasicClauseWatched::freeSize(void) const
{
    return _free.size();
}
inline Literal BasicClauseWatched::chooseFree(void) const
{
    if (_free.empty())
        exit(1); //Modifier...
    return *_free.begin();
}

inline bool BasicClauseWatched::satisfied(void) const
{
    return _satisfied;
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSE_INLINE_HH
