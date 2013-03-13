#include "Clause.hh"
#ifndef ONEWATCHEDSMARTCLAUSE_HH
#define ONEWATCHEDSMARTCLAUSE_HH

#include <vector>

class OneWatchedSmartClause HERITED_CLAUSE
{
public:
    OneWatchedSmartClause(CONSTR_ARGS(list));
    
    VIRTUAL void setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~OneWatchedClause();

    #if VERBOSE > 0
    const unsigned _number;
    #endif
protected:
    // _literals[0] est le watched
    // on a, après un setLitFalse : le watched représente la valeur de la clause, et _numOfFree est le nombre de libres de la clause (0 si clause sat)
    std::vector<Literal> _literals;
    unsigned int _numOfFree;
};


// corps des fonctions de la classe (toutes inlines)
#include "OneWatchedClause.inline.hh"

#endif //ONEWATCHEDCLAUSE_HH
