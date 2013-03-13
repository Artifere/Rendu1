#include "Clause.hh"
#ifndef BASICCLAUSEWATCHED_HH
#define BASICCLAUSEWATCHED_HH

#include <vector>

class BasicClauseWatched HERITED_CLAUSE
{
public:
    BasicClauseWatched(CONSTR_ARGS(list));
    
    VIRTUAL void setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~BasicClauseWatched();

    #if VERBOSE > 0
    const unsigned _number;
    #endif
protected:
    // les deux premers éléments du tableau sont surveillés
    // si on trouve un litéral vrai, on le surveille dans _literals[0]
    std::vector<Literal> _lits;

};


// corps des fonctions de la classe (toutes inlines)
#include "BasicClauseWatched.inline.hh"

#endif //BASICCLAUSEWATCHED_HH
