#include "Clause.hh"
#ifndef SMARTCLAUSEWATCHED_HH
#define SMARTCLAUSEWATCHED_HH

#include <vector>

class SmartClauseWatched HERITED_CLAUSE
{
public:
    SmartClauseWatched(CONSTR_ARGS(list));
    
    VIRTUAL bool setLitFalse(const Literal& l);
    VIRTUAL bool setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~SmartClauseWatched();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    // les deux premers éléments du tableau sont surveillés
    // si on trouve un litéral vrai, on le surveille dans _literals[0]
    std::vector<Literal> _lits;
    bool _watcheSecond;
};


// corps des fonctions de la classe (toutes inlines)
#include "SmartClauseWatched.inline.hh"

#endif //SMARTCLAUSEWATCHED_HH
