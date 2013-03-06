#ifndef BASICCLAUSE_HH
#define BASICCLAUSE_HH



#include "Clause.hh"
#include <stack>
#include <set>



class BasicClauseWatched : public Clause
{
    // apparamment tout doit être réécrit ici pour l'héritage avec polymorphisme
public:
    BasicClauseWatched(const std::vector<Literal>& list);

    virtual void setVar(const Literal &l);
    virtual void setLitFalse(const Literal& l);
    virtual void setLitTrue(const Literal& l);
    
    virtual void freeVar(const unsigned int varId);
    void freeLitTrue(const Literal &l);
    void freeLitFalse(const Literal &l);
    
    virtual size_t freeSize (void) const;
    virtual size_t assignedSize(void) const;
    virtual Literal chooseFree(void) const;
    virtual bool satisfied(void) const;

    virtual ~BasicClauseWatched();


protected:
    unsigned int _v1, _v2;
    varState _v1State, _v2State;
    bool _satisfied;
    std::stack<Literal> _assigned;
    std::set<Literal> _free;
};


#endif //BASICCLAUSE_HH
