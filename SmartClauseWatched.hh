#include "Clause.hh"
#ifndef SMARTCLAUSEWATCHED_HH
#define SMARTCLAUSEWATCHED_HH

#include <vector>

class SmartClauseWatched
{
public:
    SmartClauseWatched(const CONSTR_ARGS(list));
    
    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);
    
    size_t freeSize (void) const;
    Literal chooseFree(void) const;
    bool satisfied(void) const;

    ~SmartClauseWatched();

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
