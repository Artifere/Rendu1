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
    _literals.reserve(list.size());
    for (size_t i = 0; i < list.size(); ++i)
        _literals.push_back(list[i]);
    _watched1 = _literals[0];
    _watched2 = _literals[1];
}


inline void BasicClauseWatched::setLitFalse(const Literal& l, SatProblem& sp)
{
    if(! satisfied(sp))
    {
        std::vector<Literal>::iterator it;
        for (it = _literals.begin(); it != _literals.end(); ++it)
        {
            if ((it->pos() && sp._varStates[it->var()] != FALSE) || ((!it->pos()) && sp._varStates[it->var()] != TRUE))
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


inline void BasicClauseWatched::freeLitFalse(const Literal &l, SatProblem& sp)
{
    /*
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
    */
}
inline void BasicClauseWatched::freeLitTrue(const Literal &l, SatProblem& sp)
{
    /*
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
    */
}


inline size_t BasicClauseWatched::freeSize(SatProblem& sp) const
{
    varState s1 = sp._varStates[_watched1.var()],
             s2 = sp._varStates[_watched2.var()];
    if(s1 == TRUE || s2 == TRUE)
        return 0;
    if(s1 == FALSE && s2 == FALSE)
        return 0;
    if(s1 == FALSE || s2 == FALSE)
        return 1;
    return 2;
}
inline Literal BasicClauseWatched::chooseFree(SatProblem& sp) const
{
    if(sp._varStates[_watched1.var()] == FREE)
        return _watched1;
    else if(sp._varStates[_watched2.var()] == FREE)
        return _watched2;
    else
    {
        std::cout << "erreur : déduction fausse ... " << std::endl;
        return Literal();
    }
}

inline bool BasicClauseWatched::satisfied(SatProblem& sp) const
{
    std::vector<Literal>::const_iterator it = _literals.begin();
    while (it != _literals.end() && !hasSameValue(sp._varStates[_watched1.var()], _watched1.pos()))
    {
        ++it;
    }

    return (it != _literals.end());
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
