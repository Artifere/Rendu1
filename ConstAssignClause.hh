#include "Clause.hh"
#ifndef CONSTASSIGNCLAUSE_HH
#define CONSTASSIGNCLAUSE_HH

#include <stdint.h> // pour intptr_t

class ConstAssignClause
{
public:
    ConstAssignClause(const CONSTR_ARGS(list));
    
    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);
    
    size_t freeSize (void) const;
    Literal chooseFree(void) const;
    bool satisfied(void) const;

    ~ConstAssignClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    intptr_t _currentHash; // sum of the adresses of the FREE Variables contained in the ConstAssignClause
    bool _currentHashVal; // xor of all the pos() of the free literals contained in the ConstAssignClause
    Variable* _satisfied; // the adresse of the first TRUE Literal of the ConstAssignClause
    unsigned int _numOfFree; // number of FREE Variables contained in the ConstAssignClause
};





/***
 * Implementation des methodes de la classe
 * (toutes inlines)
***/


inline ConstAssignClause::ConstAssignClause(const CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE()  _currentHash((intptr_t)NULL), _currentHashVal(false), _satisfied(NULL), _numOfFree(list.size())
{
    std::vector<Literal>::const_iterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        _currentHash += (intptr_t)it->var();
        _currentHashVal = (_currentHashVal != it->pos()); // XOR booléen
        it->var()->linkToClause(it->pos(), (Clause*)this);
    }
}



inline bool ConstAssignClause::setLitFalse(const Literal& l)
{
    if(_satisfied == NULL)
    {
        _currentHash -= (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree--;
    }
    return false;
}

inline bool ConstAssignClause::setLitTrue(const Literal& l)
{
    if(_satisfied == NULL)
        _satisfied = l.var();
    return false;
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
        _currentHash += (intptr_t)l.var();
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



inline ConstAssignClause::~ConstAssignClause()
{
}


#endif //CONSTASSIGNCLAUSE_HH
