/* note : ce fichier est un fichier d'en-tête
   il est prévu pour être inclu à la fin de BasicClause.hh
   il ne doit pas être utilié seul.
 */
#include "ConstAssignClause.hh"
#ifndef CONSTASSIGNCLAUSE_INLINE_HH
#define CONSTASSIGNCLAUSE_INLINE_HH

inline ConstAssignClause::ConstAssignClause(const std::vector<Literal>& list)
    : _currentHash(0), _satisfied(0), _numOfFree(list.size())
{
    for(unsigned int i = 0; i < list.size(); i++)
    {
        _currentHash += hashOfLit(list[i]);
    }
}
   
/*
void ConstAssignClause::setVar(const Literal &l)
{
    if(_initLit.find(l) == _initLit.end())
        setLitFalse(l);
    else
        setLitTrue(l);
}*/

inline void ConstAssignClause::setLitFalse(const Literal& l)
{
    if(!_satisfied)
    {
        _currentHash -= hashOfInvertLit(l);
        _numOfFree--;
    }
}

inline void ConstAssignClause::setLitTrue(const Literal& l)
{
    if(!_satisfied)
        _satisfied = l.var();
}

/*void ConstAssignClause::freeVar(const unsigned int varID)
{
    if(!_satisfied)
    {
        _currentHash += hashOfLit(Literal(varID, _initLit.find(Literal(varID,true)) != _initLit.end()));
        _numOfFree++;
    }
    else if(_satisfied == varID)
    {
        _satisfied = 0;
    }
}*/
inline void ConstAssignClause::freeLitTrue(const Literal& l)
{
    if(_satisfied == l.var())
        _satisfied = 0;
}
inline void ConstAssignClause::freeLitFalse(const Literal& l)
{
    if(!_satisfied)
    {
        _currentHash += hashOfInvertLit(l);
        _numOfFree++;
    }
}


inline size_t ConstAssignClause::freeSize (void) const
{
    return _satisfied ? 0 : _numOfFree;
}

inline size_t ConstAssignClause::assignedSize(void) const
{
    return 0;
}

inline Literal ConstAssignClause::chooseFree(void) const
{
    return LitOfHash(_currentHash);
}
inline bool ConstAssignClause::satisfied(void) const
{
    return _satisfied;
}

inline ConstAssignClause::~ConstAssignClause() { }

#endif//CONSTASSIGNCLAUSE_INLINE_HH
