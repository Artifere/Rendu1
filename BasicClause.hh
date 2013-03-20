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




/***
 * Implementation des methodes de la classe
 * (toutes inlines)
***/
#include <cstdlib> // pour exit()


inline BasicClause::BasicClause(const CONSTR_ARGS(list))
    :  INIT_FOR_VERBOSE()  _satisfied(false), _free(list.begin(),list.end())
{
    std::vector<Literal>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it)
        it->var()->linkToClause(it->pos(), (Clause*)this);
}



inline bool BasicClause::setLitFalse(const Literal& l)
{
    if(! _satisfied)
    {
        Literal invL = l.invert();
        _free.erase(invL);
        _assigned.push(invL);
    }
    return false;
}

inline bool BasicClause::setLitTrue(const Literal& l)
{
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
    return false;
}



inline void BasicClause::freeLitTrue(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}

inline void BasicClause::freeLitFalse(const Literal &l)
{
    if (!_assigned.empty() && _assigned.top().var() == l.var())
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}



inline size_t BasicClause::freeSize(void) const
{
    return _free.size();
}

inline Literal BasicClause::chooseFree(void) const
{
    return *_free.begin();
}

inline bool BasicClause::satisfied(void) const
{
    return _satisfied;
}



inline BasicClause::~BasicClause()
{
}


#endif //BASICCLAUSE_HH
