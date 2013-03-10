#include "Clause.hh"
#ifndef ONEWATCHEDCLAUSE_HH
#define ONEWATCHEDCLAUSE_HH

#include <vector>

class OneWatchedClause HERITED_CLAUSE
{
public:
    OneWatchedClause(const std::vector<Literal>& list, unsigned int number);
    
    VIRTUAL void setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~OneWatchedClause();

    unsigned int _number;
protected:
    // invariants : _watched vaut toujours l'opposé d'un Literal de _literals
    Literal _watched;
    std::vector<Literal> _literals;
};


// corps des fonctions de la classe (toutes inlines)
#include "OneWatchedClause.inline.hh"

#endif //ONEWATCHEDCLAUSE_HH
