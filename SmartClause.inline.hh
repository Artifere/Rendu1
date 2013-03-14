/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "SmartClause.hh"
#ifndef SMARTCLAUSE_INLINE_HH
#define SMARTCLAUSE_INLINE_HH


inline SmartClause::SmartClause(CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE()  _currentHash((intptr_t)NULL), _currentHashVal(false), _satisfied(NULL), _numOfFree(list.size())
{
    //_notWatched.reserve(list.size());
    std::vector<Literal>::const_iterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        _currentHash += (intptr_t)it->var();
        _currentHashVal = (_currentHashVal != it->pos()); // XOR booléen
        it->var()->linkToClause(it->pos(), (StockedClause*)this);
    }
}


inline bool SmartClause::setLitFalse(const Literal& l)
{
    /* possible optimisation, mais n'a pas l'air de marcher
    if (_satisfied)
        _notWatched.push_back(l.invert());
    else
    {
        _currentHash -= (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree--;
    }
    return _satisfied;
    /*/
    if(_satisfied == NULL)
    {
        _currentHash -= (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree--;
        return false;
    }
    else
    {
        _notWatched.push_back(l.invert());
        return true;
    }
    // */
}
inline bool SmartClause::setLitTrue(const Literal& l)
{
    /* possible optimisation, mais n'a pas l'air de marcher
    if(_satisfied != NULL)
        _notWatched.push_back(l);
    else
        _satisfied = l.var();
    return (_satisfied != NULL);
    /*/
    if(_satisfied == NULL)
    {
        _satisfied = l.var();
        return false;
    }
    else
    {
        _notWatched.push_back(l);
        return true;
    }
    // */
}


inline void SmartClause::freeLitTrue(const Literal& l)
{
    // pas de condition : comme on arrête de surveiller un litéral après que la clause soit vraie,
    // si on appelle freeLitTrue c'est forcement sur le premier litéral qui l'a mise à vrai
    _satisfied = NULL;
    std::vector<Literal>::const_iterator it;
    for(it = _notWatched.begin(); it != _notWatched.end(); ++it)
        it->var()->linkToClause(it->pos(), (StockedClause*)this);
    _notWatched.clear();
}
inline void SmartClause::freeLitFalse(const Literal& l)
{
    // pas de condition : comme on arrête de surveiller un litéral après que la clause soit vraie,
    // si on appelle freeLitFalse, alors c'est que la clause n'est pas vraie
    _currentHash += (intptr_t)l.var();
    _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
    _numOfFree++;
}


inline size_t SmartClause::freeSize (void) const
{
    return _numOfFree;
}

inline Literal SmartClause::chooseFree(void) const
{
    return Literal((Variable*)_currentHash, _currentHashVal);
}
inline bool SmartClause::satisfied(void) const
{
    return _satisfied;
}

inline SmartClause::~SmartClause() { }

#endif//SMARTCLAUSE_INLINE_HH
