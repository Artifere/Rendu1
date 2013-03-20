#include "Clause.hh"
#ifndef ONEWATCHEDCLAUSE_HH
#define ONEWATCHEDCLAUSE_HH

#include <vector>

class OneWatchedClause
{
public:
    OneWatchedClause(const CONSTR_ARGS(list));
    
    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);
    
    size_t freeSize (void) const;
    Literal chooseFree(void) const;
    bool satisfied(void) const;

    ~OneWatchedClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    // invariants : _watched vaut toujours l'opposé d'un Literal de _literals
    Literal _watched;
    std::vector<Literal> _literals;
};


// corps des fonctions de la classe (toutes inlines)
#include "OneWatchedClause.inline.hh"

#endif //ONEWATCHEDCLAUSE_HH
