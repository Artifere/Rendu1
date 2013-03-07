/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "BasicClauseWatched.hh"
#include "satSolverWatched.hh"
#include <utility>
#ifndef BASICCLAUSEWATCHED_INLINE_HH
#define BASICCLAUSEWATCHED_INLINE_HH



inline BasicClauseWatched::BasicClauseWatched(const std::vector<Literal>& list) : _literals(list)
{
}

inline void BasicClauseWatched::setLitFalse(const Literal& l, SatProblem& sp)
{
    std::vector<Literal>::iterator it;
    for (it = _literals.begin()+2; it != _literals.end(); ++it)
    {
        if (((it->pos() && sp._varStates[it->var()] != FALSE) || ((!it->pos()) && sp._varStates[it->var()] != TRUE)))
            break;
    }
    if (it != _literals.end())
    {
        sp._toRemove.push(std::make_pair(l, this));
        if (_literals[0].var() == l.var())
            std::swap(_literals[0], *it);
        else
            std::swap(_literals[1], *it);
        sp._toInsert.push(std::make_pair(*it, this));
    }
}


inline void BasicClauseWatched::setLitTrue(const Literal& l, SatProblem& sp)
{
    if (hasSameValue( sp._varStates[_literals[0].var()],_literals[0].pos()) || hasSameValue(sp._varStates[_literals[1].var()],_literals[1].pos()))
        return;
    std::vector<Literal>::iterator it;
    for (it = _literals.begin(); it != _literals.end(); ++it)
    {
        if (it->var() == l.var())
            break;

    }
    sp._toRemove.push(std::make_pair(_literals[0], this));
    sp._toInsert.push(std::make_pair(l, this));
    std::swap(_literals[0], *it);

}



inline unsigned int BasicClauseWatched::freeSize(const SatProblem& sp) const
{
    /*for (std::vector<Literal>::const_iterator it = _literals.begin(); it != _literals.end(); ++it)
        if (sp._varStates[it->var()] == FREE)
            nb++;*/
    bool b0 = hasOppositeValue(sp._varStates[_literals[0].var()], _literals[0].pos()),
         b1 = hasOppositeValue(sp._varStates[_literals[1].var()], _literals[1].pos());

    return (b0 || b1) ? (b0 && b1 ? 0 : 1) : 2;
}

inline Literal BasicClauseWatched::chooseFree(const SatProblem& sp) const
{
    if (sp._varStates[_literals[0].var()] == FREE)
        return _literals[0];
    else
        return _literals[1];
}

inline bool BasicClauseWatched::satisfied(const SatProblem& sp) const
{
    return hasSameValue(sp._varStates[_literals[0].var()], _literals[0].pos()) || hasSameValue(sp._varStates[_literals[1].var()], _literals[1].pos());


}



inline BasicClauseWatched::~BasicClauseWatched()
{
}

#endif//BASICCLAUSEWATCHED_INLINE_HH
