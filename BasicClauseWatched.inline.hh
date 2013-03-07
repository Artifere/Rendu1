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


inline void BasicClauseWatched::setLitFalse(Literal& l, SatProblem& sp)
{
    if(! satisfied(sp))
    {
        std::vector<Literal>::iterator it;
        for (it = _literals.begin(); it != _literals.end(); ++it)
        {
            if (it->var() != _watched1.var() && it -> var() != _watched2.var() && ((it->pos() && sp._varStates[it->var()] != FALSE) || ((!it->pos()) && sp._varStates[it->var()] != TRUE)))
                break;
        }

        if (it != _literals.end())
        {
          l.invert();
          sp._toRemove.push(std::make_pair(l, this));

            if (_watched1.var() == l.var())
                _watched1 = *it;
            else
                _watched2 = *it;
            sp._toInsert.push(std::make_pair(*it, this));
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


inline size_t BasicClauseWatched::freeSize(SatProblem& sp)
{

    varState s = sp._varStates[_watched1.var()];
    //         s2 = sp._varStates[_watched2.var()];

    int nb = 0;
    for (std::vector<Literal>::iterator it = _literals.begin(); it != _literals.end(); ++it)
    {
        if (hasSameValue(sp._varStates[it->var()], it->pos()))
        {
            //std::cout << "ARGLOL" << std::endl;

            return 2;
        }
        else if (sp._varStates[it->var()] == FREE)
            nb++;
    }
    return nb;
    /*if(s1 == TRUE || s2 == TRUE)
        return 0;
    if(s1 == FALSE && s2 == FALSE)
        return 0;
    if(s1 == FALSE || s2 == FALSE)
        return 1;
    return 2;*/
}
inline Literal BasicClauseWatched::chooseFree(SatProblem& sp) const
{
    /*for(std::vector<Literal>::const_iterator it = _literals.begin(); it != _literals.end(); ++it)
        if(hasSameValue(sp._varStates[it->var()], it->pos()))
            std::cout << "babouoom" << std::endl;
        else if(sp._varStates[it->var()] == FREE)
            return *it;
    */
    
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
    while (it != _literals.end() && !hasSameValue(sp._varStates[it->var()], it->pos()))
    {
        ++it;
    }

    return (it != _literals.end());
}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
