/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClauseWatched.hh"
#include "satSolverWatched.hh"
#include <utility>
#ifndef BASICCLAUSEWATCHED_INLINE_HH
#define BASICCLAUSEWATCHED_INLINE_HH



inline BasicClauseWatched::BasicClauseWatched(const std::vector<Literal>& list)
{
    _literals.reserve(list.size());
    for (size_t i = 0; i < list.size(); ++i)
        _literals.push_back(list[i]);
    _watched1 = _literals[0];
    _watched2 = _literals[1];
}


inline void BasicClauseWatched::setLitFalse(const Literal& l, SatProblem& sp)
{
    std::vector<Literal>::iterator it;
    for (it = _literals.begin(); it != _literals.end(); ++it)
    {
        if (it->var() != l.var() && ((it->pos() && sp._varStates[it->var()] != FALSE) || ((!it->pos()) && sp._varStates[it->var()] != TRUE)))
            break;
    }
    if (it != _literals.end())
    {
        sp._toRemove.push(std::make_pair(l, this));
        if (_watched1.var() == l.var())
            _watched1 = *it;
        else
            _watched2 = *it;
        sp._toInsert.push(std::make_pair(*it, this));
    }
}


inline void BasicClauseWatched::setLitTrue(const Literal& l, SatProblem& sp)
{
    /*
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
    */
}



inline size_t BasicClauseWatched::freeSize(SatProblem& sp)
{
    unsigned int nb = 0;
    for (std::vector<Literal>::iterator it = _literals.begin(); it != _literals.end(); ++it)
        if (sp._varStates[it->var()] == FREE)
            nb++;
    return nb;
}

inline Literal BasicClauseWatched::chooseFree(SatProblem& sp) const
{
    if(sp._varStates[_watched1.var()] == FREE)
        return _watched1;
    return _watched2;
}

inline bool BasicClauseWatched::satisfied(SatProblem& sp) const
{
    std::vector<Literal>::const_iterator it = _literals.begin();
    while (it != _literals.end() && !hasSameValue(sp._varStates[it->var()], it->pos()))
        ++it;
    return (it != _literals.end());
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
