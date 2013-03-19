#include "Clause.hh"
#ifndef BASICCLAUSE_HH
#define BASICCLAUSE_HH

#include <set>
#include <stack>

class BasicClause
{
public:
    BasicClause(const CONSTR_ARGS(list));
    
    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);
    
    size_t freeSize (void) const;
    Literal chooseFree(void) const;
    bool satisfied(void) const;

    ~BasicClause();

    #if VERBOSE > 1
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
