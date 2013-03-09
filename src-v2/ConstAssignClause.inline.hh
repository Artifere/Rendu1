/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "ConstAssignClause.hh"
#ifndef CONSTASSIGNCLAUSE_INLINE_HH
#define CONSTASSIGNCLAUSE_INLINE_HH


inline ConstAssignClause::ConstAssignClause(const std::vector<Literal>& list)
    : _currentHash((long long)NULL), _currentHashVal(false), _satisfied(NULL), _numOfFree(list.size())
{
    std::vector<Literal>::const_iterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        _currentHash += (long long)it->var();
        _currentHashVal = (_currentHashVal != it->pos()); // XOR booléen
    }
}


inline void ConstAssignClause::setLitFalse(const Literal& l)
{
    if(_satisfied == NULL)
    {
        _currentHash -= (long long)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree--;
    }
}
inline void ConstAssignClause::setLitTrue(const Literal& l)
{
    if(_satisfied == NULL)
        _satisfied = l.var();
}


inline void ConstAssignClause::freeLitTrue(const Literal& l)
{
    if(_satisfied == l.var())
        _satisfied = NULL;
}
inline void ConstAssignClause::freeLitFalse(const Literal& l)
{
    if(_satisfied == NULL)
    {
        _currentHash += (long long)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree++;
    }
}


inline size_t ConstAssignClause::freeSize (void) const
{
    return _numOfFree;
}

inline Literal ConstAssignClause::chooseFree(void) const
{
    return Literal((Variable*)_currentHash, _currentHashVal);
}
inline bool ConstAssignClause::satisfied(void) const
{
    return _satisfied;
}

inline ConstAssignClause::~ConstAssignClause() { }

#endif//CONSTASSIGNCLAUSE_INLINE_HH
