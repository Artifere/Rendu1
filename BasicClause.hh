#include "Clause.hh"
#ifndef BASICCLAUSE_HH
#define BASICCLAUSE_HH

#include <set>
#include <stack>

class BasicClause HERITED_CLAUSE
{
public:
    BasicClause(CONSTR_ARGS(list));
    
    VIRTUAL bool setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~BasicClause();

    #if VERBOSE > 0
    const unsigned _number;
    #endif
protected:
    bool _satisfied;
    std::stack<Literal> _assigned;
    std::set<Literal> _free;
};


// corps des fonctions de la classe (toutes inlines)
#include "BasicClause.inline.hh"

#endif //BASICCLAUSE_HH
