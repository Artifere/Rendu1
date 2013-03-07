/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClauseWatched.hh"
#include "satSolverWatched.hh"
#ifndef BASICCLAUSEWATCHED_INLINE_HH
#define BASICCLAUSEWATCHED_INLINE_HH



inline BasicClauseWatched::BasicClauseWatched(const std::vector<Literal>& list)
{
    _satisfied = false;
    for (size_t i = 0; i < list.size(); ++i)
        _free.insert(list[i]);
}


inline void BasicClauseWatched::setLitFalse(const Literal& l, SatProblem& sp)
{
    if(! _satisfied)
    {
        std::vector<Literal>::iterator it;
        for (it = _literals.begin(); it != _literals.end(); ++it)
        {
            if ((it->pos() && sp._varStates[it->var()] != FALSE) || (!it->pos() && sp._varStates[it->var()] != TRUE))
                break;
        }

        if (it != _literals.end())
        {
            if (_watched1.var() == l.var())
                _watched1 = *it;
            else
                _watched2 = *it;
        }
    }
}

/*
inline void BasicClauseWatched::setLitTrue(const Literal& l, SatProblem& sp)
{
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
}


inline void BasicClauseWatched::freeLitFalse(const Literal &l, SatProblem& sp)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}
inline void BasicClauseWatched::freeLitTrue(const Literal &l, SatProblem& sp)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}
*/

inline size_t BasicClauseWatched::freeSize(SatProblem& sp) const
{
    bool f1 = sp._varStates[_literals[0].var()] == FREE;
    bool f2 = sp._varStates[_literals[1].var()] == FREE;
    return (b1 && b2) ? 2 : (b1 || b2 ? 1 : 0);
}
inline Literal BasicClauseWatched::chooseFree(SatProblem& sp) const
{
    if(sp._varStates[_literals[0].var()] == FREE)
        return _literals[0];
    else
        return _literals[1];
}

inline bool satisfied(SatProblem& sp) const;
{
    std::vector<Literal>::iterator it = _literals.begin();
    while (it != _literals.end() && !hasSameValue(_watched1.pos(), sp._varStates[_watched1.var()]))
    {
        ++it;
    }

    return (it != _literals.end());
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
