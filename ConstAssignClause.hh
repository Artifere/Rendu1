#include "Clause.hh"
#ifndef CONSTASSIGNCLAUSE_HH
#define CONSTASSIGNCLAUSE_HH

#include <stdint.h> // pour intptr_t

class ConstAssignClause HERITED_CLAUSE
{
public:
    ConstAssignClause(CONSTR_ARGS(list));
    
    VIRTUAL bool setLitFalse(const Literal& l);
    VIRTUAL bool setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~ConstAssignClause();

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
