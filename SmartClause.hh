#include "Clause.hh"
#ifndef SMARTCLAUSE_HH
#define SMARTCLAUSE_HH

#include <stdint.h> // pour intptr_t
#include <stack>

class SmartClause
{
public:
    SmartClause(const CONSTR_ARGS(list));
    
    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);
    
    size_t freeSize (void) const;
    Literal chooseFree(void) const;
    bool satisfied(void) const;

    ~SmartClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    intptr_t _currentHash; // sum of the adresses of the FREE Variables contained in the ConstAssignClause
    bool _currentHashVal; // xor of all the pos() of the free literals contained in the ConstAssignClause
    bool _satisfied; // the adresse of the first TRUE Literal of the ConstAssignClause
    unsigned int _numOfFree; // number of FREE Variables contained in the ConstAssignClause
    std::vector<Literal> _notWatched;
};


// corps des fonctions de la classe (toutes inlines)
#include "SmartClause.inline.hh"

#endif //SMARTCLAUSE_HH
