#ifndef BASICCLAUSEWATCHED_HH
#define BASICCLAUSEWATCHED_HH

#include <vector>
#include <set>
#include <stack>

class BasicClauseWatched HERITED_CLAUSE
{
public:

    BasicClauseWatched(const std::vector<Literal>& list);
    
    VIRTUAL void setLitFalse(const Literal& l, SatProblem& sp);
    VIRTUAL void setLitTrue(const Literal& l, SatProblem& sp);
    
    VIRTUAL unsigned int freeSize (const SatProblem& sp) const;
    VIRTUAL Literal chooseFree(const SatProblem& sp) const;
    VIRTUAL bool satisfied(const SatProblem& sp) const;
    
    VIRTUAL ~BasicClauseWatched();


protected:
    std::vector<Literal> _literals;

};


// corps des fonctions de la classe (toutes inlines)
#include "BasicClauseWatched.inline.hh"

#endif //BASICCLAUSEWATCHED_HH
