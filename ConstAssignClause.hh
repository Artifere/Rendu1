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


// corps des fonctions de la classe (toutes inlines)
#include "ConstAssignClause.inline.hh"

#endif //CONSTASSIGNCLAUSE_HH
